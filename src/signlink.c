/*
 * Copyright (C) 2021-2022 subjectivelyobjective
 *
 * This file is part of rs2client317.
 *
 * rs2client317 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * rs2client317 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "client.h"
#include "signlink.h"
#include "util/util.h"

/********************************** Globals *********************************/
bool signlink_reporterror = true;
/****************************************************************************/

static const char *POSSIBLE_LOCS[] = {
	"c:/windows/", "c:/winnt/", "d:/windows/", "d:/winnt/", "e:/windows/",
	"e:/winnt/", "f:/windows/", "f:/winnt/", "c:/", "~/", "/tmp/", "",
	"c:/rscache", "/rscache"
};

bool dir_exists(char *dirstr)
{
	DIR* loc = opendir(dirstr);
	bool exists = loc != NULL;
	
	if (loc)
		closedir(loc);

	return exists;
}

int put_home_dir(char *dest)
{
	if (dest == NULL)
		return -1;

	char *homedir = getenv("HOME");

	if (homedir == NULL) {
		uid_t uid = getuid();
		char *strbuf;
		struct passwd pwbuf;
		struct passwd *pw = NULL;
		size_t strbuflen = (size_t) sysconf(_SC_GETPW_R_SIZE_MAX);

		if ((strbuf = calloc(1, strbuflen)) == NULL)
			return -1;

		if (	getpwuid_r(uid, &pwbuf, strbuf, strbuflen, &pw) != 0 ||
			pw == NULL) {
				free(strbuf);
				return -1;
		}

		memcpy(dest, pw->pw_dir, strnlen(pw->pw_dir, strbuflen) + 1);
		free(strbuf);
	} else {
		memcpy(dest, homedir, strnlen(homedir, get_path_max()) + 1);
	}

	return 0;
}

size_t get_path_max(void)
{
	size_t path_max = 0;
	const long _PATH_MAX = pathconf(".", _PC_PATH_MAX);

	if (_PATH_MAX == -1)
		path_max = 1024;
	else if (_PATH_MAX > 10240)
		path_max = 10240;
	else
		path_max = _PATH_MAX;

	return path_max;
}

int signlink_findcachedir(signlink *sign)
{
	if (sign == NULL)
		return -1;

	if (sign->storeid < 32 || sign->storeid > 34)
		sign->storeid = 32;

	const size_t PATH_MAX = get_path_max();

	const size_t FILE_STORE_LEN = strnlen(CACHE_DIR, PATH_MAX) + 2;
	char file_store_dir[FILE_STORE_LEN + 1];
	snprintf(file_store_dir, FILE_STORE_LEN + 1, CACHE_DIR"%u",
					sign->storeid);

	for (size_t i = 0; i < ARRAY_SIZE(POSSIBLE_LOCS); i++) {
		char candidate_parent[PATH_MAX];
		size_t loc_len = 0;

		if (strncmp(POSSIBLE_LOCS[i], "~/", PATH_MAX) == 0) {
			int res = put_home_dir(candidate_parent);

			if (res)
				return res;

			loc_len = strnlen(candidate_parent, PATH_MAX);
		} else if (strncmp(POSSIBLE_LOCS[i], "", PATH_MAX) == 0) {
			char cwd[PATH_MAX];

			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				loc_len = strnlen(cwd, PATH_MAX);
				memcpy(candidate_parent, cwd, loc_len + 1);
			} else {
				continue;
			}
 		} else {
			loc_len = strnlen(POSSIBLE_LOCS[i], PATH_MAX);
			memcpy(
				candidate_parent, POSSIBLE_LOCS[i],
				loc_len + 1);
		}

		if (!dir_exists(candidate_parent))
			continue;

		char last_ch = candidate_parent[loc_len - 1];

		if (last_ch != '/') {
			candidate_parent[loc_len++] = '/';
			candidate_parent[loc_len] = '\0';
		}

		memcpy(
			candidate_parent + loc_len, file_store_dir,
			FILE_STORE_LEN + 1);
		loc_len += FILE_STORE_LEN;

		if (mkdir(candidate_parent, 0777) == 0 || errno == EEXIST) {
			candidate_parent[loc_len++] = '/';
			candidate_parent[loc_len] = '\0';
			sign->cache_dir = strndup(candidate_parent, loc_len);
			return 0;
		}
	}

	return -1;
}

signlink *signlink_new(void)
{
	signlink *sign = (signlink *) malloc(sizeof(signlink));

	if (sign != NULL) {
		sign->cache_dir = NULL;
		sign->storeid = 32;
		sign->midifade = 0;
		sign->midi[0] = '\0';
	}

	return sign;
}

void signlink_free(signlink *sign)
{
	if (sign != NULL) {
		free(sign->cache_dir);
		free(sign);
	}
}

int signlink_run(signlink *sign)
{
	if (sign != NULL)
		signlink_findcachedir(sign);

	return 0;
}

