#ifndef EDITLINE_H_
#define EDITLINE_H_


#define CTL(x)          ((x) & 0x1F)
#define ISCTL(x)        ((x) && (x) < ' ')
#define UNCTL(x)        ((x) + 64)
#define META(x)         ((x) | 0x80)
#define ISMETA(x)       ((x) & 0x80)
#define UNMETA(x)       ((x) & 0x7F)

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    CSdone = 0,                 
    CSeof,                      
    CSmove,
    CSdispatch,
    CSstay,
    CSsignal
} el_status_t;

/* Editline specific types, despite rl_ prefix.  From Heimdal project. */
typedef int rl_list_possib_func_t(char*, char***);
typedef el_status_t el_keymap_func_t(void);
typedef int  rl_hook_func_t(void);
typedef int  rl_getc_func_t(void);
typedef void rl_voidfunc_t(void);
typedef void rl_vintfunc_t(int);
typedef void rl_vcpfunc_t(char *);

/* FSF Readline compat tupes */
typedef char  *rl_complete_func_t   (char *, int*);
typedef char  *rl_compentry_func_t  (const char *, int);
typedef char **rl_completion_func_t (const char *, int, int);

/* Display 8-bit chars "as-is" or as `M-x'? Toggle with M-m. (Default:0 - "as-is") */
extern int rl_meta_chars;

/* Editline specific functions. */
extern char *      el_find_word(void);
extern void        el_print_columns(int ac, char **av);
extern el_status_t el_ring_bell(void);
extern el_status_t el_del_char(void);

extern el_status_t el_bind_key(int key, el_keymap_func_t function);
extern el_status_t el_bind_key_in_metamap(int key, el_keymap_func_t function);

extern const char *el_next_hist(void);
extern const char *el_prev_hist(void);

extern char       *rl_complete(char *token, int *match);
extern int         rl_list_possib(char *token, char ***av);
extern char      **rl_completion_matches(const char *token, rl_compentry_func_t *generator);
extern char       *rl_filename_completion_function(const char *text, int state);

/* For compatibility with FSF readline. */
extern int         rl_point;
extern int         rl_mark;
extern int         rl_end;
extern int         rl_inhibit_complete;
extern char       *rl_line_buffer;
extern const char *rl_readline_name;
extern FILE       *rl_instream;  /* The stdio stream from which input is read. Defaults to stdin if NULL - Not supported yet! */
extern FILE       *rl_outstream; /* The stdio stream to which output is flushed. Defaults to stdout if NULL - Not supported yet! */
extern int         el_no_echo;   /* E.g under emacs, don't echo except prompt */
extern int         el_no_hist;   /* Disable auto-save of and access to history -- e.g. for password prompts or wizards */
extern int         el_hist_size; /* size of history scrollback buffer, default: 15 */

extern void  rl_initialize      (void);
extern void  rl_reset_terminal  (const char *terminal_name);
extern void  rl_uninitialize    (void);

extern void  rl_save_prompt     (void);
extern void  rl_restore_prompt  (void);
extern void  rl_set_prompt      (const char *prompt);

extern void  rl_clear_message   (void);
extern void  rl_forced_update_display(void);

extern void  rl_prep_terminal   (int meta_flag);
extern void  rl_deprep_terminal (void);

extern int   rl_getc(void);
extern int   rl_insert_text     (const char *text);
extern int   rl_refresh_line    (int ignore1, int ignore2);

extern char *readline           (const char *prompt);

extern void  add_history        (const char *line);
extern int   read_history       (const char *filename);
extern int   write_history      (const char *filename);

extern rl_getc_func_t *rl_set_getc_func(rl_getc_func_t *func);

extern rl_completion_func_t  *rl_attempted_completion_function;
extern rl_complete_func_t    *rl_set_complete_func    (rl_complete_func_t *func);
extern rl_list_possib_func_t *rl_set_list_possib_func (rl_list_possib_func_t *func);

/* Alternate interface to plain readline(), for event loops */
extern void rl_callback_handler_install (const char *prompt, rl_vcpfunc_t *lhandler);
extern void rl_callback_read_char       (void);
extern void rl_callback_handler_remove  (void);

#ifdef __cplusplus
}
#endif

#endif  /* EDITLINE_H_ */
