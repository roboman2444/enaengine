#ifndef GAMECODEINCLUDESHEADER
#define GAMECODEINCLUDESHEADER

	typedef struct ecallheader_s {
		//console
		int (*console_printf)(const char *fmt, ...);
		int (*console_nprintf)(const size_t size, const char *fmt, ...);

		//cvar
		int (*cvar_register)(cvar_t *c);
		int (*cvar_unregister)(const int id);
		int (*cvar_findByNameRINT)(char * name);
		void (*cvar_nameset)(char * name, const char *value);
		void (*cvar_pset)(cvar_t *c, const char *value);
		void (*cvar_idset)(const int id, const char *value);
		cvar_t * (*cvar_returnById)(const int id);
		cvar_t * (*cvar_findByNameRPOINT)(char * name);

		//entity
		entity_t *(*entity_findByNameRPOINT)(const char *name);
		entitylistpoint_t (*entity_findAllByNameRPOINT)(const char *name);
		entitylistint_t (*entity_findAllByNameRINT)(const char *name);
		entity_t * (* entity_returnById)(const int id);
		entity_t * (* entity_addRPOINT)(const char * name);
		int (*entity_findByNameRINT)(const char *name);
		int (* entity_addRINT)(const char * name);
		int (*entity_delete)(const int id); //todo do i want to have a markfordelete in the flags?

		//file
		int (*file_loadString)(const char * filename, char ** output, int * length, const int debugmode);
		int (*file_loadStringNoLength)(const char * filename, char ** output, const int debugmode);

		//stringlib
		unsigned int (*string_toVec)(const char *s, vec_t *c, const unsigned int maxdem);


		//todo
	} ecallheader_t;

	typedef struct gcallheader_s {
		//todo
	} gcallheader_t;

#endif
