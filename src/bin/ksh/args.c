/*
 * Copyright (c) 2016 Henning Matyschok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sh.h"

/*
 * XXX: This file should be understand as workaround regarding
 * XXX: partially refactoring activities targeting current 
 * XXX: implementation of ksh(1).
 * XXX:
 * XXX: Intentionally, I'll add more by strdup(3) performed 
 * XXX: copies of as constant declared and initialized string
 * XXX: literals.
 * XXX:
 * XXX: Further, refactoring takes place sooner or later.
 */
 
#include <err.h>
#include <stdlib.h>
#include <sysexits.h>

/*
 * During shell initialization used literals.
 */

static char *icms_cat_cmd;
static char *icms_cc_cmd;
static char *icms_chmod_cmd;
static char *icms_cp_cmd;
static char *icms_date_cmd;
static char *icms_ed_cmd;
static char *icms_emacs_cmd;
static char *icms_grep_cmd;
static char *icms_ls_cmd;
static char *icms_mail_cmd;
static char *icms_make_cmd;
static char *icms_mv_cmd;
static char *icms_pr_cmd;
static char *icms_rm_cmd;
static char *icms_sh_cmd;
static char *icms_sed_cmd;
static char *icms_vi_cmd;
static char *icms_who_cmd;

static char *icms_dash_i;
static char *icms_dash_r;
static char *icms_dash_tu;
static char *icms_dash_x;

static char *icms_home;
static char *icms_ksh_version;
static char *icms_optind;
static char *icms_path;
static char *icms_ppid;
static char *icms_shell;

static char *icms_typeset_random_mailcheck;

static char *icms_hash_alias;
static char *icms_type_alias;
static char *icms_stop_alias;
static char *icms_autoload_alias;
static char *icms_functions_alias;
static char *icms_history_alias;
static char *icms_integer_alias;
static char *icms_nohup_alias;
static char *icms_local_alias;
static char *icms_r_alias;
static char *icms_login_alias;

char *initcoms[55] = { 0 };

char *initifs = NULL;
char *initsubs = NULL;

/*
 * Various commands and options.
 */
 
char *alias_cmd = NULL; 
char *eval_cmd = NULL;
char *let_cmd = NULL;
char *read_cmd = NULL;
char *set_cmd = NULL;  
char *typeset_cmd = NULL;
char *unalias_cmd = NULL;

char *cu_arg_options = NULL;
char *ct_arg_options = NULL; 
char *read_reply = NULL;
char *read_options = NULL;

/*
 * By typeset accepted literals.
 */

char *typeset_arg_optarg = NULL;
char *typeset_arg_columns = NULL;
char *typeset_arg_lines = NULL;
char *typeset_arg_underscore = NULL;

/*
 * By p_time accepted literals.
 */

char *p_time_ws = NULL;
char *p_time_nl = NULL;
char *p_time_real = NULL;
char *p_time_user = NULL;
char *p_time_sys = NULL;
char *p_time_system_nl = NULL;

/*
 * Holds copy of string denotes shell name.
 */
 
char *_ksh_cmd = NULL;
char *_ksh_name = NULL;

/*
 * Holds copy of string denotes ksh(1) version.
 */
char *_ksh_version = NULL;
char *_ksh_version_param = NULL;

/*
 * Used by restr_com[] as argv for shcomexec during main.
 */	
 
char *rc_arg_options = NULL;
char *rc_arg_path = NULL;
char *rc_arg_env = NULL;
char *rc_arg_shell = NULL;

char *_root = NULL;

/*
 * Used as fmt-string by shf_vfprintf.
 */

char *_shf_null_fmt = NULL;

/*
 * Release by initargs bound ressources.
 */
static void 	
args_atexit(void)
{
	free(alias_cmd);
	free(eval_cmd);
	free(let_cmd);
	free(read_cmd);
	free(set_cmd); 
	free(typeset_cmd);
	free(unalias_cmd);  
	
	free(read_options);
	free(read_reply);
	
	free(cu_arg_options);
	free(ct_arg_options); 
	
	free(typeset_arg_optarg);
	free(typeset_arg_columns);
	free(typeset_arg_lines);
	free(typeset_arg_underscore);	
		
	free(p_time_ws);
	free(p_time_nl);
	free(p_time_real);
	free(p_time_user);
	free(p_time_sys);
	free(p_time_system_nl);
	
	free(_ksh_cmd);
	free(_ksh_name);
	free(_ksh_version);
	free(_ksh_version_param);
	
	free(initifs);
	free(initsubs);
	
	free(icms_dash_i);
	free(icms_dash_r);
	free(icms_dash_tu);
	free(icms_dash_x);
	
	free(icms_home);
	free(icms_ksh_version);
	free(icms_optind);
	free(icms_path);
	free(icms_ppid);
	free(icms_shell);
	
	free(icms_typeset_random_mailcheck);

	free(icms_hash_alias);
	free(icms_type_alias);
	free(icms_stop_alias);
	free(icms_autoload_alias);
	free(icms_functions_alias);
	free(icms_history_alias);
	free(icms_integer_alias);
 	free(icms_nohup_alias);
 	free(icms_local_alias);
 	free(icms_r_alias);
	free(icms_login_alias);
	
	free(icms_cat_cmd);
	free(icms_cc_cmd);
	free(icms_chmod_cmd);
	free(icms_cp_cmd);
	free(icms_date_cmd);
	free(icms_ed_cmd);
	free(icms_emacs_cmd);
	free(icms_grep_cmd);

	free(icms_ls_cmd);
	free(icms_mail_cmd);
	free(icms_make_cmd);
	free(icms_mv_cmd);
	free(icms_pr_cmd);
	free(icms_rm_cmd);
	free(icms_sed_cmd);
	
	free(icms_sh_cmd);
	free(icms_vi_cmd);
	free(icms_who_cmd);
	
	free(rc_arg_options);
	free(rc_arg_path);
	free(rc_arg_env);
	free(rc_arg_shell);
	
	free(_root);
	free(_shf_null_fmt);
}

void 
initargs(void)
{
	if (atexit(args_atexit) < 0)
		err(EX_OSERR, "%s", strerror(errno));
	
	alias_cmd = strdup("alias");
	eval_cmd = strdup("eval");
	let_cmd = strdup("let");
	read_cmd = strdup("read");
	set_cmd = strdup("set");  
	typeset_cmd = strdup("typeset");
	unalias_cmd = strdup("unalias");
	
	read_options = strdup("-r");
	read_reply = strdup("REPLY");
	
	ct_arg_options = strdup("-"); 
	cu_arg_options = strdup("-ta");

	typeset_arg_optarg = strdup("OPTARG");
	typeset_arg_columns = strdup("COLUMNS");
	typeset_arg_lines = strdup("LINES");
	typeset_arg_underscore = strdup("_");	

	p_time_ws = strdup(" ");
	p_time_ws = strdup("\n");
	p_time_real = strdup(" real ");
	p_time_user = strdup(" user ");
	p_time_sys = strdup("sys  ");
	p_time_system_nl = strdup(" system\n");
	
	_ksh_cmd = strdup("ksh");
	_ksh_name = strdup(kshname);	
	_ksh_version = strdup(ksh_version);
	_ksh_version_param = strdup("SH_VERSION");

	initifs = strdup("IFS= \t\n");
	initsubs = strdup("${PS2=> } ${PS3=#? } ${PS4=+ }");

	icms_cat_cmd = strdup("cat");
	icms_cc_cmd = strdup("cc");
	icms_chmod_cmd = strdup("chmod");
	icms_cp_cmd = strdup("cp");
	icms_date_cmd = strdup("date");
	icms_ed_cmd = strdup("ed");
	icms_emacs_cmd = strdup("emacs");
	icms_grep_cmd = strdup("grep");
	icms_ls_cmd = strdup("ls");
	icms_mail_cmd = strdup("mail");
	icms_make_cmd = strdup("make");
	icms_mv_cmd = strdup("mv");
	icms_pr_cmd = strdup("pr");
	icms_rm_cmd = strdup("rm");
	icms_sed_cmd = strdup("sed");
	icms_sh_cmd = strdup("sh");
	icms_vi_cmd = strdup("vi");
	icms_who_cmd = strdup("who");

	icms_dash_i = strdup("-i");
	icms_dash_r = strdup("-r");
	icms_dash_tu = strdup("-tu");
	icms_dash_x = strdup("-x");
	
	icms_home = strdup("HOME");
	icms_ksh_version = strdup("KSH_VERSION");
	icms_optind = strdup("OPTIND=1");
	icms_path = strdup("PATH");
	icms_ppid = strdup("PPID");
	icms_shell = strdup("SHELL");
	
	icms_typeset_random_mailcheck = strdup("typeset -i RANDOM "
		"MAILCHECK=\"${MAILCHECK-600}\" "
		"SECONDS=\"${SECONDS-0}\" "
		"TMOUT=\"${TMOUT-0}\"");
	
	icms_hash_alias = strdup("hash=alias -t");
	icms_type_alias = strdup("type=whence -v");	
	icms_stop_alias = strdup("stop=kill -STOP");
	icms_autoload_alias = strdup("autoload=typeset -fu");
	icms_functions_alias = strdup("functions=typeset -f");
	icms_history_alias = strdup("history=fc -l");
 	icms_integer_alias = strdup("integer=typeset -i");
 	icms_nohup_alias = strdup("nohup=nohup ");
 	icms_local_alias = strdup("local=typeset");
 	icms_r_alias = strdup("r=fc -e -");
 	icms_login_alias = strdup("login=exec login");
/*
 * Vector used during shell initialization.
 */
	initcoms[0] = typeset_cmd; 
	initcoms[1] = icms_dash_r; 
	initcoms[2] = icms_ksh_version; 
	initcoms[3] = NULL;
	initcoms[4] = typeset_cmd; 
	initcoms[5] = icms_dash_x; 
	initcoms[6] = icms_shell; 
	initcoms[7] = icms_path; 
	initcoms[8] = icms_home; 
	initcoms[9] = NULL;
	initcoms[10] = typeset_cmd; 
	initcoms[11] = icms_dash_i; 
	initcoms[12] = icms_ppid; 
	initcoms[13] = NULL;
	initcoms[14] = typeset_cmd; 
	initcoms[15] = icms_dash_i; 
	initcoms[16] = icms_optind; 
	initcoms[17] = NULL;
	initcoms[18] = eval_cmd; 
	initcoms[19] = icms_typeset_random_mailcheck; 
	initcoms[20] = NULL;
	initcoms[21] = alias_cmd;
/* 
 * Standard ksh aliases.
 */
	initcoms[22] = icms_hash_alias;   
	initcoms[23] = icms_type_alias;
	initcoms[24] = icms_stop_alias;
	initcoms[25] = icms_autoload_alias;
	initcoms[26] = icms_functions_alias;
	initcoms[27] = icms_history_alias;
	initcoms[28] = icms_integer_alias;
	initcoms[29] = icms_nohup_alias;
	initcoms[30] = icms_local_alias;
	initcoms[31] = icms_r_alias;
/* 
 * Aliases that are builtin commands in at&t.
 */
	initcoms[32] = icms_login_alias;
	initcoms[33] = NULL;
/* 
 * This is what at&t ksh seems to track; with the addition of emacs. 
 */
	initcoms[34] = alias_cmd; 
	initcoms[35] = icms_dash_tu;
	initcoms[36] = icms_cat_cmd; 
	initcoms[37] = icms_cc_cmd; 
	initcoms[38] = icms_chmod_cmd; 
	initcoms[39] = icms_cp_cmd; 
	initcoms[40] = icms_date_cmd; 
	initcoms[41] = icms_ed_cmd; 
	initcoms[42] = icms_emacs_cmd; 
	initcoms[43] = icms_grep_cmd; 
	initcoms[44] = icms_ls_cmd;
	initcoms[45] = icms_mail_cmd; 
	initcoms[46] = icms_make_cmd; 
	initcoms[47] = icms_mv_cmd; 
	initcoms[48] = icms_pr_cmd; 
	initcoms[49] = icms_rm_cmd; 
	initcoms[50] = icms_sed_cmd; 
	initcoms[51] = icms_sh_cmd; 
	initcoms[52] = icms_vi_cmd; 
	initcoms[53] = icms_who_cmd;
	initcoms[54] = NULL;
	initcoms[55] = NULL;

	rc_arg_options = strdup("-r");
	rc_arg_path = strdup("PATH");
	rc_arg_env = strdup("ENV");
	rc_arg_shell = strdup("SHELL");
	
	_root = strdup("root");
	_shf_null_fmt = strdup("(null %s)");
	
	if (errno == ENOMEM) 
		err(EX_OSERR, "%s", strerror(errno));
}

