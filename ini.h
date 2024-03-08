#ifndef __ini__
#define __ini__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct options
{
	int max_secs;
	int max_sec_name_len;
	int max_vars_in_sec;
	int max_var_name_len;
	int max_var_value_len;
};

struct variable
{
	char* name;
	char* value;
};

struct section
{
	char* name;
	struct variable* vars;
	int nvars;
};

struct ini
{
	struct section* secs;
	int nsecs;
};

int ini_parse(struct ini* data, char* buffer, struct options* opt);
void ini_release(struct ini* data);

char* pchar_getval(struct ini* data, char* section, char* variable);
int int_getval(struct ini* data, char* section, char* variable);
double double_getval(struct ini*, char* section, char* variable);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ini__