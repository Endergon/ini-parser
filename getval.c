#include "ini.h"

#ifndef NO_STD_LIB

#include <string.h>
#include <stdlib.h>

char* pchar_getval(struct ini* data, char* section, char* variable)
{
	int i, j;
	for (i = 0; i < data->nsecs; i++)
	{
		if (!strcmp(data->secs[i].name, section)) break;
	}
	for (j = 0; j < data->secs[i].nvars; j++)
	{
		if (!strcmp(data->secs[i].vars[j].name, variable)) break;
	}
	return data->secs[i].vars[j].value;
}

int int_getval(struct ini* data, char* section, char* variable)
{
	char* buffer = pchar_getval(data, section, variable);
	return atoi(buffer);
}

double double_getval(struct ini* data, char* section, char* variable)
{
	char* buffer = pchar_getval(data, section, variable);
	return atof(buffer);
}

#else

static int strcmp(const char* s1, const char* s2)
{
	while (*s1 == *s2)
	{
		if ((*s1 == '\0') || (*s2 == '\0')) break;
		s1++; s2++;
	}
	return *s1-*s2;
}

static int atoi_v(struct variable* var)
{
	int result = 0;
	int sign = 1;
	
	char* s = var->value;
	
	if (*s == '-')
	{
		sign = -1;
		s++;
	}
	else if (*s == '+')	s++;
	
	while (*s != '\0')
	{
		result = result * 10 + *s++ - '0';
	}
	
	result *= sign;

	return result;
}

static double atof_v(struct variable* var)
{
	double result = 0.0;
	int sign = 1;
	
	char* s = var->value;
	
	if (*s == '-')
	{
		sign = -1;
		s++;
	} else if (*s == '+') s++;
	
	while ((*s != '\0') && (*s != '.') && (*s != 'e') && (*s != 'E'))
	{
		result = result * 10 + *s++ - '0';
	}
	
	if (*s == '.')
	{
		int power = 10;
		s++;
		while ((*s != '\0') && (*s != 'e') && (*s != 'E'))
		{
			result = result + (double)(*s++ - '0')/power;
			power *= 10;
		}
	}
	
	if ((*s == 'e') || (*s == 'E'))
	{
		int power = 0;
		int psign = 1;
		s++;
		if (*s == '-')
		{
			psign = -1;
			s++;
		} else if (*s == '+') s++;
		
		while (*s != '\0')
		{
			power = power + *s++ - '0';
		}
		if (psign == 1)
		{
			while (power-- > 0) result *= 10;
		}
		else if (psign == -1)
		{
			while (power-- > 0) result /= 10;
		}
	}
	
	result *= sign;
	
	return result;
}

static struct variable* search_val(struct ini* data, char* section, char* variable)
{
	int i, j;
	for (i = 0; i < data->nsecs; i++)
	{
		if (!strcmp(data->secs[i].name, section)) break;
	}
	for (j = 0; j < data->secs[i].nvars; j++)
	{
		if (!strcmp(data->secs[i].vars[j].name, variable)) break;
	}
	
	return &data->secs[i].vars[j];
}

char* pchar_getval(struct ini* data, char* section, char* variable)
{
	return (search_val(data, section, variable))->value;
}

int int_getval(struct ini* data, char* section, char* variable)
{
	return atoi_v(search_val(data, section, variable));
}

double double_getval(struct ini* data, char* section, char* variable)
{
	return atof_v(search_val(data, section, variable));
}

#endif