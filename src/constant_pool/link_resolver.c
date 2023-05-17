#include "mini_jvm/constant_pool/link_resolver.h"

#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/error/err_vm.h"

err_vm cp_expect(c_pool *cp, uint16_t index, uint8_t type, cp_info *ret_val) {
    if (index >= cp->size || index == 0)
        return E_ICFE;
    
    cp_info entry = cp->pool[index];
    if (entry.tag != type)
        return E_ICFE;

    *ret_val = entry;
    return E_SUCC;
}

err_vm cp_get_class_name(c_pool *cp, uint16_t index, UChar **ret_val) {
    cp_info class;
    cp_info utf8;
    E_PROP(cp_expect(cp, index, CONSTANT_Class, &class));
    E_PROP(cp_expect(cp, class.data.class.name_index, CONSTANT_Utf8, &utf8));

    *ret_val = utf8.data.utf8.bytes;
    return E_SUCC;
}
