#include "mini_jvm/constant_pool/link_resolver.h"

#include <glib.h>

#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/error/err_vm.h"

err_vm cp_expect(GPtrArray *cp, uint16_t id, uint8_t type, cp_info *out) {
    if (id >= cp->len || id == 0)
        return E_ICFE;
    
    cp_info *entry = g_ptr_array_index(cp, id);
    if (entry->tag != type)
        return E_ICFE;

    *out = *entry;
    return E_SUCC;
}

err_vm cp_get_class_name(GPtrArray *cp, uint16_t id, cp_utf8 *out) {
    cp_info class;
    cp_info utf8;
    E_PROP(cp_expect(cp, id, CONSTANT_Class, &class));
    E_PROP(cp_expect(cp, class.data.class.name_index, CONSTANT_Utf8, &utf8));

    *out = utf8.data.utf8;
    return E_SUCC;
}
