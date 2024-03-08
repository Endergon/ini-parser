#ifdef WINDOWS_HEAP_ALLOC

#include <windows.h>
#define ALLOCATE(x,y) (x*)HeapAlloc(hHeap, 0, sizeof(x) * y)
#define RELEASE(x,y,z) HeapFree(hHeap, 0, x)

#elifdef POSIX_MMAP

#include <unistd.h>
#include <sys/mman.h>
#define ALLOCATE(x,y) (x*)mmap(0, sizeof(x) * y, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0)
#define RELEASE(x,y,z) munmap(x, sizeof(y) * z)

#else

#include <malloc.h>
#define ALLOCATE(x,y) (x*)malloc(sizeof(x) * y)
#define RELEASE(x,y,z) free(x)

#endif

#include "ini.h"

#define isAlpha(c) ((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z'))
#define isSpace(c) ((c == ' ') || (c == '\t') || (c == '\n'))
#define isNumber(c) ((c >= '0') && (c <= '9'))
#define isSymbol(c) ((c == '_') || (c == '.') || (c == '$'))
#define isComment(c) ((c == ';') || (c == '#'))
#define isEndOfLine(c) ((c == '\n') || (c == '\0') || isComment(c))

#define HANDLE_ERROR(c) if (c == NULL) return -1;
	
enum mode
{
	DETERMINE, SECTION, VARIABLE, COMMENT, BREAK
};

enum read_expr_v
{
	VARIABLE_NAME, VARIABLE_VALUE
};

static enum mode state = DETERMINE;
static enum read_expr_v reader = VARIABLE_NAME;

#ifdef WINDOWS_HEAP_ALLOC
static HANDLE hHeap;
#endif

int ini_parse(struct ini* data, char* buffer, struct options* opt)
{
	#ifdef WINDOWS_HEAP_ALLOC
	hHeap = GetProcessHeap();
	#endif
	
	char* p = buffer;
	
	int isecs = -1;
	int ivars = -1;
	int idx = 0;
	
	data->secs = ALLOCATE(struct section, opt->max_secs);
	HANDLE_ERROR(data->secs);
	data->nsecs = 0;
	
	for (; state != BREAK; p++)
	{
		switch (state)
		{
			case DETERMINE:
			{
				if (*p == '[')
				{
					state = SECTION;
					isecs++;
					ivars = -1;
					data->nsecs++;
					data->secs[isecs].nvars = 0;
					data->secs[isecs].name = ALLOCATE(char, opt->max_sec_name_len);
					HANDLE_ERROR(data->secs[isecs].name);
					data->secs[isecs].vars = ALLOCATE(struct variable, opt->max_vars_in_sec);
					HANDLE_ERROR(data->secs[isecs].vars);
				}
				else if (isAlpha(*p) || isSymbol(*p))
				{
					state = VARIABLE;
					ivars++;
					data->secs[isecs].nvars++;
					data->secs[isecs].vars[ivars].name = ALLOCATE(char, opt->max_var_name_len);
					HANDLE_ERROR(data->secs[isecs].vars[ivars].name);
					data->secs[isecs].vars[ivars].value = ALLOCATE(char, opt->max_var_value_len);
					HANDLE_ERROR(data->secs[isecs].vars[ivars].value);
					p--;
				}
				else if (isComment(*p)) state = COMMENT;
				else if (*p == '\0') state = BREAK;
			}
			break;
			
			case SECTION:
			{
				if (*p == ']')
				{
					state = DETERMINE;
					data->secs[isecs].name[idx] = '\0';
					idx = 0;
				}
				else data->secs[isecs].name[idx++] = *p;
			}
			break;
			
			case VARIABLE:
			{
				if (isEndOfLine(*p))
				{
					state = DETERMINE;
					reader = VARIABLE_NAME;
					data->secs[isecs].vars[ivars].value[idx] = '\0';
					idx = 0;
				}
				else if (*p == '=')
				{
					reader = VARIABLE_VALUE;
					data->secs[isecs].vars[ivars].name[idx] = '\0';
					idx = 0;
				}
				else if (reader == VARIABLE_NAME)
				{
					data->secs[isecs].vars[ivars].name[idx++] = *p;
				}
				else if (reader == VARIABLE_VALUE)
				{
					data->secs[isecs].vars[ivars].value[idx++] = *p;
				}
				
			}
			break;
			
			case COMMENT:
			{
				if (isEndOfLine(*p)) state = DETERMINE;
			}
			break;
		}
	}
	return 0;
}

void ini_release(struct ini* data)
{
	for (int i = 0; i < data->nsecs; i++)
	{
		for (int j = 0; j < data->secs[i].nvars; j++)
		{
			RELEASE(data->secs[i].vars[j].name, char, opt->max_var_name_len);
			RELEASE(data->secs[i].vars[j].value, char, opt->max_var_value_len);
		}
		RELEASE(data->secs[i].name, char, opt->max_sec_name_len);
		RELEASE(data->secs[i].vars, struct variable, opt->max_vars_in_sec);
	}
	RELEASE(data->secs, struct section, opt->max_secs);
}