/*******************************************************************************
* FILE PURPOSE: Debug Extensions to MXP
*               
********************************************************************************
* FILE NAME:    mxdbg.h
*
* DESCRIPTION:  
*
* (C) Copyright 1999 Telogy Networks, Inc.
*******************************************************************************/

#ifndef _MXDBG_H_
#define _MXDBG_H_

#include "swDef.h"
#include "rtxErr.h"

/*******************************************************************************
* Type:         MX_DBG_FORMAT_CONTEXT_T
*
********************************************************************************
* Description:  Provides print and parsing format context.
*
*******************************************************************************/
typedef struct
{
    BOOL    little_endian;      /* TRUE to use little endian format */
    BOOL    big_endian;         /* TRUE to use big endian format    */
                                /* if both FALSE use native format  */
    SINT    size;               /* size of data items: 8, 16, or 32 */
    SINT    radix;              /* default radix: 2, 8, 10, or 16   */

    UINT32  flags;              /* see below */

    /* for dump */
    SINT    width;              /* number of values per line */
    CHAR*   line_prefix;        /* prefix for each line */
    SINT    addr_radix;         /* radix (base) for the address */
    UINT32  addr_flags;         /* flags for address */
    SINT    addr_min_digits;    /* minumum number of address digits */
    CHAR*   addr_suffix;        /* address and data seperator string */
    SINT    bytes_per_addr;     /* number of bytes per address */
    CHAR*   line_suffix;        /* end of line suffix */
} MX_DBG_FORMAT_CONTEXT_T;

#define MX_DBG_FORMAT_NO_DEFAULTS       0x0001  /* if flags is 0 defaults are
                                                ** used, setting this bit  
                                                ** allows a zero value for all
                                                ** options bits w/o using the
                                                ** defaults */
#define MX_DBG_FORMAT_PREFIX            0x0002  /* print a radix prefix */
#define MX_DBG_FORMAT_UNDERSCORES       0x0004  /* seperate groups of digits
                                                ** with underscores (groups are
                                                ** 3 or 4 based on radix */
#define MX_DBG_FORMAT_SUFFIX            0x0008  /* print a radix suffix */
#define MX_DBG_FORMAT_ASCII             0x0010  /* print ascii dump in 
                                                ** XdbgDataDump */

/*******************************************************************************
* Type:         MX_DBG_MEM_MAP_ENTRY_T
*
********************************************************************************
* Description:  Provides parsing context to XdbgParseValueList.
*
*******************************************************************************/
#define MX_DBG_MEM_ACCESS_READ                  0x0001
#define MX_DBG_MEM_ACCESS_WRITE                 0x0002
#define MX_DBG_MEM_ACCESS_8BIT                  0x0010
#define MX_DBG_MEM_ACCESS_16BIT                 0x0020
#define MX_DBG_MEM_ACCESS_32BIT                 0x0040
#define MX_DBG_MEM_ACCESS_16BIT_ALIGN8          0x0100
#define MX_DBG_MEM_ACCESS_32BIT_ALIGN8          0x0200
#define MX_DBG_MEM_ACCESS_32BIT_ALIGN16         0x0400

#define MX_DBG_MEM_ACCESS_RW                    \
    (MX_DBG_MEM_ACCESS_WRITE | MX_DBG_MEM_ACCESS_READ)

#define MX_DBG_MEM_ACCESS_ALL_OP_ALIGNED        \
    (                                           \
          MX_DBG_MEM_ACCESS_8BIT                \
        | MX_DBG_MEM_ACCESS_16BIT               \
        | MX_DBG_MEM_ACCESS_32BIT               \
    )

#define MX_DBG_MEM_ACCESS_ALL_OP_UNALIGNED      \
    (                                           \
          MX_DBG_MEM_ACCESS_16BIT_ALIGN8        \
        | MX_DBG_MEM_ACCESS_32BIT_ALIGN8        \
        | MX_DBG_MEM_ACCESS_32BIT_ALIGN16       \
    )
      
#define MX_DBG_MEM_ACCESS_ALL_ALIGNED           \
    (                                           \
          MX_DBG_MEM_ACCESS_ALL_OP_ALIGNED      \
        | MX_DBG_MEM_ACCESS_RW                  \
    )
      
#define MX_DBG_MEM_ACCESS_ALL                   \
    (                                           \
          MX_DBG_MEM_ACCESS_ALL_OP_ALIGNED      \
        | MX_DBG_MEM_ACCESS_ALL_OP_UNALIGNED    \
        | MX_DBG_MEM_ACCESS_RW                  \
    )
      
typedef struct
{
    const CHAR*     name;           /* short name for region (15 char)
                                    ** an entry with a null name terminates 
                                    ** the list of entries in a map */
    UINT32          start_addr;     /* start address (target space) */
    UINT32          length;         /* length of region */
    UINT32          flags;          /* access type flags */
    const CHAR*     description;    /* description (40 char) */
} MX_DBG_MEM_MAP_ENTRY_T;

/*******************************************************************************
* Type:         MX_DBG_MEM_CONTEXT_T
*
********************************************************************************
* Description:  Provides parsing context to XdbgParseValueList.
*
*******************************************************************************/
/* forward declaration of type */
typedef struct MX_DBG_MEM_CONTEXT_tag MX_DBG_MEM_CONTEXT_T;

typedef MX_Result MX_DBG_MEM_BUFFER_FUNC_T(
    MX_DBG_MEM_CONTEXT_T*       p_mem_context,
    void*                       buffer
);

typedef MX_Result MX_DBG_MEM_BUFFER_LEN_FUNC_T(
    MX_DBG_MEM_CONTEXT_T*       p_mem_context,
    void*                       buffer,
    UINT32                      len
);

typedef MX_Result MX_DBG_MEM_FUNC_T(
    MX_DBG_MEM_CONTEXT_T*       p_mem_context
);

typedef MX_Result MX_DBG_MEM_ADDR_FUNC_T(
    MX_DBG_MEM_CONTEXT_T*       p_mem_context,
    UINT32                      src_addr,
    UINT32                      src_len
);

/* now define structure body */
struct MX_DBG_MEM_CONTEXT_tag
{
    UINT32                          addr;           /* address of operation */
    UINT32                          len;            /* length of operation */
    SINT                            size;           /* bit size of operation
                                                    ** 0, 8, 16, or 32 */
    SINT                            type;           /* test type etc. */

    void*                           p_user_context; /* for use by memory access
                                                    ** functions */
    /* 
    ** this mem list is used to persist find patterns from command to command
    ** if p_mem_list is NULL, the pattern will not be persisted
    */
    void*                           p_mem_list;
    UINT32                          mem_list_size;
    UINT32                          max_mem_list_size;

    const MX_DBG_MEM_MAP_ENTRY_T*   p_map;
    MX_DBG_FORMAT_CONTEXT_T*        p_format;

    MX_DBG_MEM_BUFFER_FUNC_T*       mem_read;
    MX_DBG_MEM_BUFFER_LEN_FUNC_T*   mem_write;    
    MX_DBG_MEM_BUFFER_LEN_FUNC_T*   mem_find;    
    MX_DBG_MEM_ADDR_FUNC_T*         mem_move;    
    MX_DBG_MEM_FUNC_T*              mem_test;    
};

/*******************************************************************************
* Type:         MX_DBG_MEM_DEF_CONTEXT_T
*
********************************************************************************
* Description:  User context type needed for default implementation of Memory
*               read and write functions.
*
*******************************************************************************/
typedef struct
{
    UINT32      addr_min;
    UINT32      addr_max;

    UINT32      offset;
} MX_DBG_MEM_DEF_CONTEXT_T;

/*******************************************************************************
* Type:         <MX_DIGIT_ constants>
*
********************************************************************************
* Description:  Special values returned by XdbgDigitValue
*
*******************************************************************************/
#define MX_DIGIT_IGNORE     -1
#define MX_DIGIT_INVALID    -2

/*******************************************************************************
* Type:         <MX_DBG_FOUND_ constants>
*
********************************************************************************
* Description:  Flag values for XdbgMemParseAddrLenAssign
*
*******************************************************************************/
#define MX_DBG_FOUND_ADDR   0x0001
#define MX_DBG_FOUND_LEN    0x0002
#define MX_DBG_FOUND_ASSIGN 0x0004

/*******************************************************************************
* Constant:     MX_DBG_NUM_BUFFER_SIZE
*
********************************************************************************
* Description:  Maximum size of buffer needed when calling XdbgFormatU/SInt.
*
*******************************************************************************/
#define MX_DBG_NUM_BUFFER_SIZE                              \
    (                                                       \
           2   /* suffix or prefix */                       \
        +  32  /* digits (binary) */                        \
        +  8   /* underscores */                            \
        +  1   /* sign */                                   \
        +  1   /* null character termination */             \
    )

/*******************************************************************************
* Type:         MX_DBG_CMD_ENTRY_T
*
********************************************************************************
* Description:  
*
*******************************************************************************/
#define MX_DBG_CMD_NAME_MAX_LEN     255

typedef UINT16 MX_DBG_CMD_MODE_T;
#define MX_DBG_CMD_MODE_SET         1
#define MX_DBG_CMD_MODE_SHOW        2
#define MX_DBG_CMD_MODE_LIST        3
#define MX_DBG_CMD_MODE_HELP        4
#define MX_DBG_CMD_MODE_RESET       5

typedef UINT32 MX_DBG_CMD_TYPE_T;
#define MX_DBG_CMD_TYPE_CMD_OLD     1  /* Old style command function */
#define MX_DBG_CMD_TYPE_CMD_ARGV    2  /* New style command functions w/ ARGV */
#define MX_DBG_CMD_TYPE_CMD_PARSE   3  /* New style command w/ structure bind */
#define MX_DBG_CMD_TYPE_CMD_TABLE   4  /* Command(/var) sub table */

                                       /* Variable types: */
#define MX_DBG_CMD_TYPE_VAR_UINT32  5  /* integer types */
#define MX_DBG_CMD_TYPE_VAR_SINT32  6
#define MX_DBG_CMD_TYPE_VAR_UINT16  7
#define MX_DBG_CMD_TYPE_VAR_SINT16  8
#define MX_DBG_CMD_TYPE_VAR_UINT8   9
#define MX_DBG_CMD_TYPE_VAR_SINT8   10
#define MX_DBG_CMD_TYPE_VAR_STR_P   11  /* string pointer */
#define MX_DBG_CMD_TYPE_VAR_STR_BUF 12  /* string buffer */

#define MX_DBG_CMD_TYPE_VAR_ENUM    MX_DBG_CMD_TYPE_VAR_SINT32

typedef UINT32 MX_DBG_CMD_OPT_T;

/* direct or indirect via instance values */
#define MX_DBG_CMD_OPT_INST_MASK    0x00000007
#define MX_DBG_CMD_OPT_DIRECT       0x00000000  
#define MX_DBG_CMD_OPT_INST1        0x00000001  
#define MX_DBG_CMD_OPT_INST2        0x00000002
#define MX_DBG_CMD_OPT_INST3        0x00000003
#define MX_DBG_CMD_OPT_INST4        0x00000004
#define MX_DBG_CMD_OPT_INST5        0x00000005
#define MX_DBG_CMD_OPT_INST6        0x00000006
#define MX_DBG_CMD_OPT_INST_CUR     0x00000007  
#define MX_DBG_CMD_OPT_INDIRECT     0x00000008  

/* force radix options */
#define MX_DBG_CMD_OPT_BIN          0x00000010  
#define MX_DBG_CMD_OPT_OCT          0x00000020
#define MX_DBG_CMD_OPT_DEC          0x00000040
#define MX_DBG_CMD_OPT_HEX          0x00000080

/* enum support */
/* value should be one of the defined enum constants */
#define MX_DBG_CMD_OPT_ENUM                 0x00000100  

/* value should be enum names or numeric value */
#define MX_DBG_CMD_OPT_ENUM_OR_NUM          0x00000200  

/* value should be one or more enum constants */
#define MX_DBG_CMD_OPT_ENUM_BITMAP          0x00000400  

/* value should be one or more enum constants or numeric value */
#define MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP   0x00000800  

#define MX_DBG_CMD_OPT_ENUM_MASK            (MX_DBG_CMD_OPT_ENUM             | \
                                            MX_DBG_CMD_OPT_ENUM_OR_NUM       | \
                                            MX_DBG_CMD_OPT_ENUM_BITMAP       | \
                                            MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP)

#define MX_DBG_CMD_OPT_ENUM_BITMAP_MASK    (MX_DBG_CMD_OPT_ENUM_BITMAP       | \
                                            MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP)


/* parallel bitmap/super structure support */

/* structure has a valid bitmap */
#define MX_DBG_CMD_OPT_VALID_BITMAP 0x00001000  

/* valid bitmap is 32-bit */
#define MX_DBG_CMD_OPT_32BIT_BITMAP 0x00002000  

/* super structure has pointer to this struct */
#define MX_DBG_CMD_OPT_VALID_PTR    0x00004000  

/* parse function options */

/* cmd_func will be called to parse */
#define MX_DBG_CMD_OPT_CUSTOM_PARSE 0x00010000  

/* vars: cmd_func will be called to read/write */
#define MX_DBG_CMD_OPT_CUSTOM_RW    0x00020000  

/* cmd_func will be called for help */
#define MX_DBG_CMD_OPT_CUSTOM_HELP  0x00040000  

/* Forward declaration */
typedef struct MX_DBG_CMD_ENTRY_tag     MX_DBG_CMD_ENTRY_T;
typedef struct MX_DBG_CMD_CONTEXT_tag   MX_DBG_CMD_CONTEXT_T;

typedef MX_Result 
MX_DBG_CMD_PARSE_FUNC_T
(
    MX_DBG_CMD_CONTEXT_T*   p_context,
    MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
    char**                  pp_cmd_line
);

/* enumeration table */
typedef struct MX_DBG_ENUM_ENTRY_tag
{
    const char*                     name;
    UINT32                          value;
    const char*                     short_description;
} MX_DBG_ENUM_ENTRY_T;

typedef int MD_DBG_GEN_FUNC_T(int, ...);

typedef union MX_DBG_PARAM_tag
{
    UINT32                          v_uint32;
    void*                           v_void_ptr;
    UINT32                          v_func_ptr;
} MX_DBG_PARAM_T;

struct MX_DBG_CMD_ENTRY_tag
{
    const char*                 name;       /* cmd/var parse name */
    MX_DBG_CMD_TYPE_T           type;       /* cmd/var type, see above */
    MX_DBG_CMD_OPT_T            options;    /* cmd/var options, see above */
    UINT32                      location;   /* pointer for direct, offset for 
                                            ** indirect */
    SINT32                      gen_param1; /* general parameter: location of 
                                            ** valid_param */
    UINT32                      gen_param2; /* general parameter: valid_param 
                                            ** value (bitmap) */
    UINT32                      min_value;  /* int: minimum value, 
                                            ** string: min length
                                            ** cmd: minimum argc */
    UINT32                      max_value;  /* int: max value, 
                                            ** string: max length
                                            ** cmd: max argc */
    UINT32                      def_value;  /* int: default value, 
                                            ** string: ptr to def value or NULL
                                            ** cmd: pointer to default arg 
                                            ** string or NULL */
    void*                       format;     /* cmd: format table 
                                            ** int: enum table
                                            ** string: valid chars
                                            */
    MX_DBG_CMD_PARSE_FUNC_T*    cmd_func;   /* command for parsing and execution 
                                            ** vars: can be NULL, uses default 
                                            ** for cmd, will be execution
                                            */
    const char*                 show_desc;  /* show command string */
    const char*                 syntax_str; /* command syntax string */
    const char*                 short_desc; /* 20 - 40 character description */
    const char*                 long_desc;  /* paragraph of help */
};

typedef UINT32 MX_DBG_CMD_TABLE_OPT_T;
#define MX_DBG_ACCESS_ALL           0x00000000  /* any one can access */
#define MX_DBG_ACCESS_GUEST         0x00000001  /* login as guest or higher */
#define MX_DBG_ACCESS_ADMIN         0x00000002  /* login as admin or higher */
#define MX_DBG_ACCESS_ROOT          0x00000003  /* login as root or higher */
#define MX_DBG_ACCESS_NONE          0x00000004  /* no one can access */
#define MX_DBG_ACCESS_MASK          0x0000000F  /* mask for above values */
#define MX_DBG_CMD_TABLE_OPT_HIDE   0x00000010  /* hide even if accessable */
#define MX_DBG_CMD_TABLE_OPT_SHOW   0x00000020  /* show even if not accessable*/
#define MX_DBG_CMD_TABLE_OPT_FLAT   0x00000100  /* treat cmds as part of 
                                                   upper context */
#define MX_DBG_CMD_TABLE_OLD_STYLE  0x00010000  /* command table is in old 
                                                   style */
#define MX_DBG_CMD_TABLE_DEF_HELP   0x00100000  /* default action is to give 
                                                   short help */
#define MX_DBG_CMD_TABLE_DEF_FOCUS  0x00200000  /* default action is make 
                                                   curent top level table */

typedef struct MX_DBG_CMD_TABLE_tag
{
    MX_DBG_CMD_TABLE_OPT_T      options;        /* table options */
    UINT32                      max_entries;    /* 0 = no additions allowed, 
                                                ** N maximum number of 
                                                ** entries in table */
    MX_DBG_CMD_ENTRY_T**        p_entries;
} MX_DBG_CMD_TABLE_T;

typedef struct MX_DBG_CMD_TABLE_LIST_tag
{
    UINT32                      max_entries;
    MX_DBG_CMD_TABLE_T**        p_cmd_tables;
} MX_DBG_CMD_TABLE_LIST_T;

#define MX_DBG_CMD_CONTEXT_MAX_DEPTH    10
#define MX_DBG_CMD_CONTEXT_INIT_INST    (UINT8)-1
#define MX_DBG_CMD_CONTEXT_STR_LENGTH   256
struct MX_DBG_CMD_CONTEXT_tag
{
    /* current command table at this point in parsing */
    MX_DBG_CMD_TABLE_T*         p_cur_cmd_tbl;

    /* indirect access instance pointers */
    UINT8                       cur_inst;
    char                        syntax_str[MX_DBG_CMD_CONTEXT_STR_LENGTH];
    char                        cmd_line_str[MX_DBG_CMD_CONTEXT_STR_LENGTH];
    MX_DBG_CMD_ENTRY_T*         p_valid_cmd_entry;
    void*                       inst[MX_DBG_CMD_CONTEXT_MAX_DEPTH];
};

typedef struct MX_DBG_CMD_CONTEXT_TABLE_tag
{
    char*                           name;       /* context parse name */
    const MX_DBG_CMD_CONTEXT_T*     p_cntxt;    /* pointer to the context */
} MX_DBG_CMD_CONTEXT_TABLE_T;

typedef struct MX_DBG_CMD_GLOBAL_CONTEXT_tag
{
    /* context history */
    UINT32                      max_context_depth;
    UINT32                      cur_context_depth;
    UINT32                      next_context_depth;
    MX_DBG_CMD_CONTEXT_T*       p_first_context;

    /* global command table */
    MX_DBG_CMD_TABLE_T*         p_global_cmd_tbl;

    /* global var table */
    MX_DBG_CMD_TABLE_T*         p_global_var_tbl;

    /* current user access rights */
    MX_DBG_CMD_TABLE_OPT_T      cur_access;
} MX_DBG_CMD_GLOBAL_CONTEXT_T;

/*******************************************************************************
* Type:         MX_DBG_SPY_HOOK_FUNC_T
*
********************************************************************************
* Description:  Type of function that can get installed as a Xspy hook function
*
*               A function of this type may be installed via the XspyHookInstall
*               function.
*
*               If installed this function will be called after Xspy has 
*               determied that the trace should be generated by consulting the 
*               spy key table and has expanded the % sequences from the Xspy 
*               callers format string.
*
* Returns : TRUE - The default spy message queuing and send to std out via SPY 
*                  TASK should/will be suppressed
*           FALSE - Default operation after this spyHook function returns
*
*******************************************************************************/
#if 0
typedef BOOL MX_DBG_SPY_HOOK_FUNC_T
(
    MX_SpyKey_t     SpyKey,         /* Spy Key passed by Xspy caller */
    MX_SpyLevel_t   SpyLevel,       /* Spy level passed by Xspy caller */
    const char*     msg             /* Message passed by Xspy caller w/ 
                                    ** % sequences expanded.  The message
                                    ** is null terminated.  The message does not
                                    ** include timestamp, end of lines
                                    ** characters, or any other additions made 
                                    ** by the Xspy function itself */
);
#endif
/*******************************************************************************
* Function:      XspyHookInstall
*
********************************************************************************
* Description:  This function installs a Xspy hook (extension) function.
*               Only one hook maybe installed at a time.  The function returns 
*               the value of any previously intalled hook, or NULL if none.
*               It is valid to "install" a NULL function, which has the effect 
*               of disabling a previously installed hook.
*
*******************************************************************************/
/* MX_DBG_SPY_HOOK_FUNC_T* XspyHookInstall(MX_DBG_SPY_HOOK_FUNC_T* hook_func);*/

/*******************************************************************************
* Prototypes
*******************************************************************************/
CHAR* 
XdbgParseUint32(
    const CHAR* string,
    SINT        default_radix,
    UINT32*     p_value
);

CHAR* 
XdbgParseSint32(
    const CHAR* string,
    SINT        default_radix,
    SINT32*     p_value
);

SINT
XdbgFormatUint32(
    UINT32      value,
    SINT        radix,
    SINT        min_digits,
    UINT32      flags,
    CHAR*       p_buffer
);

SINT
XdbgFormatSint32(
    SINT32      value,
    SINT        radix,
    SINT        min_digits,
    UINT32      flags,
    CHAR*       p_buffer
);

void XdbgFormatDefaults(
    MX_DBG_FORMAT_CONTEXT_T*        p_format
);

SINT
XdbgParseFormatList(
    SINT                            argc, 
    CHAR*                           argv[], 
    MX_DBG_FORMAT_CONTEXT_T*        p_format
);

SINT 
XdbgParseValueList(
    SINT                            argc, 
    CHAR*                           argv[], 
    MX_DBG_FORMAT_CONTEXT_T*        p_format,
    SINT*                           p_mem_size,
    void*                           p_mem
);

void
XdbgDataDump(
    UINT32                          addr,
    UINT32                          len,
    void*                           p_data,
    MX_DBG_FORMAT_CONTEXT_T*        p_format
);

MX_Result XdbgPrintf(const CHAR* fmt, ...);

void XdbgMemCmd(
    SINT                    argc, 
    CHAR*                   argv[], 
    MX_DBG_MEM_CONTEXT_T*   p_mem_context
);

void XdbgMemCmdHelp(void);

SINT XdbgMemParseAddrLenAssign(
    SINT                        argc, 
    CHAR*                       argv[],
    UINT32*                     p_flags,
    UINT32*                     p_addr,
    UINT32*                     p_len,
    UINT32*                     p_list_size,
    void*                       p_list,
    MX_DBG_FORMAT_CONTEXT_T*    p_format
);

MX_Result XdbgFopen(const CHAR* filename, const CHAR* mode, void** p_handle);
MX_Result XdbgFread(void* handle, void* buffer, UINT32* p_size);
MX_Result XdbgFwrite(void* handle, void* buffer, UINT32 size);
MX_Result XdbgFclose(void* handle);

BOOL XdbgIsLittleEndian(void);
SINT XdbgDigitValue(CHAR digit_char);
SINT XdbgNumDigits(SINT num_bits, SINT radix);

MX_DBG_MEM_BUFFER_FUNC_T            XdbgMemDefReadFunc;
MX_DBG_MEM_BUFFER_LEN_FUNC_T        XdbgMemDefWriteFunc;
MX_DBG_MEM_BUFFER_LEN_FUNC_T        XdbgMemDefFindFunc;    
MX_DBG_MEM_ADDR_FUNC_T              XdbgMemDefMoveFunc;
MX_DBG_MEM_FUNC_T                   XdbgMemDefTestFunc;

/* typedef void* MX_DBG_MEMCPY_FUNC_T(void* p_dest, const void* p_src, 
                                      size_t len);
   MX_DBG_MEMCPY_FUNC_T    XdbgMemCpy8;
   MX_DBG_MEMCPY_FUNC_T    XdbgMemCpy16;
   MX_DBG_MEMCPY_FUNC_T    XdbgMemCpy32;
 */

/* New additions */
extern MX_DBG_CMD_MODE_T mx_dbg_cmd_mode;
extern MX_DBG_CMD_TYPE_T mx_dbg_last_cmd_type;


MX_Result 
XdbgCmdNextArg
(
    char**                      pp_cmd_line, 
    char*                       p_cmd
); 

MX_Result
XdbgCmdNextVar
(
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    char*                       p_cmd_name
);

MX_Result
XdbgCmdNextNum
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT32*                     p_val
);

MX_Result 
XdbgCmdGetPtr
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    void**                      pp_var
);

MX_Result XdbgContextPush(MX_DBG_CMD_CONTEXT_T* p_context, void* p_var);
MX_Result XdbgContextPop(MX_DBG_CMD_CONTEXT_T* p_context, void** p_var);
MX_Result XdbgContextWrite(MX_DBG_CMD_CONTEXT_T* p_context, void* p_var);
MX_Result XdbgContextRead(MX_DBG_CMD_CONTEXT_T* p_context, void** p_var);

MX_DBG_CMD_ENTRY_T*
XdbgLookupCmd
(
    MX_DBG_CMD_TABLE_T*         p_cmd_tbl,
    char*                       p_cmd_name
);

MX_Result XdbgLookupEnumBitmap
(
    MX_DBG_ENUM_ENTRY_T*        p_enum_tbl,
    char*                       p_cmd_name, 
    UINT32*                     p_val,
    char**                      pp_cmd_line
);

MX_Result
XdbgLookupEnumBitmapName
(
    MX_DBG_ENUM_ENTRY_T*        p_enum_tbl,
    UINT32                      val,
    char*                       p_str
);

MX_Result 
XdbgCmdParse
(
    MX_DBG_CMD_CONTEXT_T*       p_context,     
    MX_DBG_CMD_ENTRY_T*         p_prev_cmd_entry,
    char**                      pp_cmd_line
);

MX_Result XdbgCmdExec(MX_DBG_CMD_CONTEXT_T* p_context, char** pp_line);
MX_Result XdbgCmdHelp(MX_DBG_CMD_CONTEXT_T* p_context, char* p_cmd_name);

MX_Result 
XdbgCmdSyntaxShow
(    
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry 
);

MX_Result 
XdbgCmdParseShow
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_prev_cmd_entry, 
    char**                      pp_cmd_line,
    char**                      pp_line
);

MX_Result
XdbgCmdRead
(
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    void*                       p_var,
    char*                       p_str
);

BOOL
XdbgCmdValidParamsCheck
(
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    void*                       p_var
);

MX_Result XdbgCmdTblFunc
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line
);

MX_Result XdbgCmdVarFunc
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line
);

MX_Result
XdbgCmdVarBufferSet
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT16                      length,
    void*                       p_buffer,
    UINT16                      num_bits
);

MX_Result
XdbgCmdVarBufferShow
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT16                      length,
    void*                       p_buffer,
    UINT16                      num_bits
);

MX_Result
XdbgCmdVarBufferReset
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT16                      length,
    void*                       p_buffer,
    UINT16                      num_bits
);

void
XdbgCmdVarCopy
(
    MX_DBG_CMD_CONTEXT_T*       p_context_src,
    MX_DBG_CMD_CONTEXT_T*       p_context_dst,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    BOOL                        copy
);

void
XdbgCmdTblCopy
(
    MX_DBG_CMD_CONTEXT_T*       p_context_src,
    MX_DBG_CMD_CONTEXT_T*       p_context_dst,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    BOOL                        copy
);

void
XdbgCmdValidParamsSet
(
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    void*                       p_var,
    BOOL                        set_valid_params
);

MX_Result
XdbgLookupEnum
(
    MX_DBG_ENUM_ENTRY_T*        p_enum_tbl,
    char*                       p_cmd_name, 
    UINT32*                     p_val
);

MX_Result
XdbgLookupEnumName
(
    MX_DBG_ENUM_ENTRY_T*        p_enum_tbl,
    UINT32                      val,
    char*                       p_str
);

void
XdbgCmdSyntaxGet
(
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char*                       p_str        
);

MX_Result
XdbgCmdVarFuncBitfield
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT32                      mask
);

MX_Result
XdbgCmdVarFuncBitfieldShift
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT32                      mask,
    UINT16                      shift
);

MX_Result
XdbgCmdPtrFunc
(
    MX_DBG_CMD_CONTEXT_T*       p_context,
    MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
    char**                      pp_cmd_line,
    UINT16                      size
);

#endif
