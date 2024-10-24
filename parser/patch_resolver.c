#include "patch_resolver.h"



size_t ps_to_modified_index(const PatchString* restrict ps, size_t index_){
    ptrdiff_t index = index_;
    const Array* patches = &ps->patches;

    const struct _Patch* patch = patches->ptr;
    const struct _Patch* patchEnd = patches->ptr + patches->size;
    
    for (; patch <= patchEnd; patch++){
        if (index < patch->start);
        else if (index >= patch->end) index += patch->len_mod; 
        else if (patch->len_mod < 0)
            index = patch->start;
    }
    
    return index > 0 ? index : 0;
}
size_t ps_to_original_index(const PatchString* restrict ps, size_t index_){
    ptrdiff_t index = index_;
    const Array* patches = &ps->patches;
    const struct _Patch* patch = patches->ptr;
    const struct _Patch* patchEnd = patches->ptr + patches->size;

    for (; patchEnd >= patch; patchEnd--){
        if (index < patchEnd->start);
        else if (patchEnd->len_mod > 0 && index < patchEnd->start + patchEnd->len_mod)
            index = patchEnd->start -1;
        else if (index >= patchEnd->start)
            index -= patchEnd->len_mod;
    }
    return index > 0 ? index : 0;
}


bool ps_insert_at(PatchString* ps, size_t point, char* str_to_insert){
    debug_assert(str_to_insert != NULL);
    
    Array* istr = &ps->internal_string;
    const size_t old_size = istr->size;
    const size_t addition_size = strlen(str_to_insert);
    const size_t new_len = old_size + addition_size;

    debug_assert(point <= old_size);
    
    if (point > old_size) return 0;

    array_grow_for(istr, new_len);
    memmove(point + addition_size + istr->ptr, point + istr->ptr, old_size-point);
    memcpy(point + istr->ptr, str_to_insert, addition_size);
    
    array_push(&ps->patches, ((struct _Patch){
        point,
        point,
        addition_size
    }));

    istr->size = new_len;
    return 1;
}

bool ps_delete(PatchString* restrict ps, size_t point, size_t amount){
    Array* istr = &ps->internal_string;
    debug_assert(istr->size >= amount + point);
    if (istr->size < amount + point) return 0;



    char* start_deletion = point + istr->ptr;
    char* end_deletion = start_deletion + amount;
    const struct _Patch deletion = {
        point,
        point + amount,
        -amount
    };

    
    memmove(start_deletion, end_deletion, istr->size - point - amount);
    istr->size -= amount;

    array_push(&ps->patches, deletion);
    return 1;
}

void ps_reconstruct_using(PatchString* ps, uint8_t window_size, const WindowPredicate predicate){
    PatchString new_patch_str = _ps_windowed_construction(
        ps->internal_string.ptr,
        ps->internal_string.size,
        window_size,
        predicate
    );

    array_push_from_other(&ps->patches, &new_patch_str.patches);
    array_free(&new_patch_str.patches);
    array_free(&ps->internal_string);

    ps->internal_string = new_patch_str.internal_string;
}





PatchString _ps_windowed_construction(
        const char* restrict source, 
        const size_t source_len, 
        const uint8_t window_size, 
        const WindowPredicate predicate){
    predicate(NULL);
    
    size_t old_source = (size_t) source;

    PatchString new = {
        array_new_with_capacity(char, source_len),

        // A good power of two estimate
        array_new_with_capacity(struct _Patch, 16)
    };

    for (int i = 0; i <= window_size; i++) if (!source[i]){
        array_push_ptr(&new.internal_string, source, i);
        return new;
    }
    size_t pseudo_index = 0;
    while (source[window_size]){
        ConstructionResult cr = predicate(source);
        switch (cr.variety){
            case CR_KEEP:
                KEEP:
                array_push(&new.internal_string, *source);
                source++;
                pseudo_index++;
                break;
            case CR_DISCARD_AND_INSERT_OWNED:
            case CR_DISCARD_AND_INSERT:
                debug_assert(cr.insertion);

                const int size = strlen(cr.insertion);

                array_push_ptr(&new.internal_string, cr.insertion, size);
                array_push(&new.patches, ((struct _Patch){
                    pseudo_index,
                    pseudo_index,
                    size
                }));
                pseudo_index += size;
                if ( CR_DISCARD_AND_INSERT_OWNED == cr.variety ) free((void*) cr.insertion);

                // Continue to handle discard
            case CR_DISCARD:
                if (cr.amount == 0) goto KEEP;
                array_push(&new.patches, ((struct _Patch){
                    pseudo_index,
                    pseudo_index + cr.amount,
                    -cr.amount
                }));
                source += cr.amount;
                if ((size_t) source > old_source + source_len) return new;
                break;
                
            default:
        }
    }
    if ((size_t) source < old_source + source_len)
        array_push_ptr(&new.internal_string, source, old_source + source_len - (size_t) source);
    return new;

}