#include "register_types.h"

#include "core/class_db.h"
#include "gdskia.h"

void register_skia_types() {
	ClassDB::register_class<GdSkia>();
}

void unregister_skia_types() {
   // nothing to do here
}
