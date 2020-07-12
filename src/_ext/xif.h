/**
 * \file: xif.h
 * \brief: xif(XIF) module.
 */

#ifndef _XIF_H_
#define _XIF_H_

/*
 * xif: interface between apps, which is a FIFO queue, including command and return queue.
 */

#include "projectdefs.h"

// Import: XIF_MESSAGE_MAX_LENGTH
#include "drivers.h"

////###############################################################################################
#define __code_block_xif_TxifMessage 1
#if __code_block_xif_TxifMessage

// the size of header of TxifMessage
#define XIF_MESSAGE_HEADER_LENGTH (sizeof(unsigned short)*2 + sizeof(unsigned char)*4)
// the size of ToidPrm
#define XIF_MESSAGE_DATA_LENGTH (XIF_MESSAGE_MAX_LENGTH - (XIF_MESSAGE_HEADER_LENGTH))

/*
 * ToidPrm: parameters of TxifMessage
 * NOTE:
 * The same data type as TVariant only but the different size(see member bytes[XIF_MESSAGE_DATA_LENGTH]).
 * In another words, the ToidPrm is the long size version of TVariant.
 * The ToidPrm is designed for functions in xifsrv/xifclt modules. the TgetFunctionPtr/TsetFunctionPtr function
 * pointers in the outside modules has many instances, they are implemented so many places, so the parameter
 * of data should has a short size version, that is TVariant.
 */
typedef union ToidPrm {
	int intValue;
	int intValues[1];

	unsigned int uintValue;
	unsigned int uintValues[1];

	long longValue;
	long longValues[1];

	unsigned long ulongValue;
	unsigned long ulongValues[1];

	short shortValue;
	short shortValues[1];

	unsigned short ushortValue;
	unsigned short ushortValues[1];

	float floatValue;
	float floatValues[1];

	void *ptrValue;
	void *ptrValues[1];

	char charValue;
	char charValues[1];

	unsigned char ucharValue;
	unsigned char ucharValues[1];

	char chars[XIF_MESSAGE_DATA_LENGTH];
	unsigned char bytes[XIF_MESSAGE_DATA_LENGTH];
} ToidPrm;

typedef struct TxifMessage {
	unsigned short length; // length of message
	unsigned char sender; // the app number of sender
	unsigned char command;
	unsigned char status; // status
	unsigned char oid;
	unsigned short index;
	ToidPrm prm;
} TxifMessage;

/*
 * defined for TxifMessage.command
 */
#define XIF_COMMAND_NULL	-1
// GET/SET Value of the OID with its data type
#define XIF_COMMAND_GET	0
#define XIF_COMMAND_SET	1

// GET/SET Values of the OID with string type
#define XIF_COMMAND_GETS	2
#define XIF_COMMAND_SETS	3

// GET/SET Properties of OID
#define XIF_COMMAND_GET_OID	4
#define XIF_COMMAND_GETS_OID	10 // for test
#define XIF_COMMAND_SET_OID	5

// GET/SET Values of multiple OID with its data type
#define XIF_COMMAND_MGET	6 // (Reserved)
#define XIF_COMMAND_MSET	7 // (Reserved)
// GET/SET Values of multiple OID with string type
#define XIF_COMMAND_MGETS	8 // (Reserved)
#define XIF_COMMAND_MSETS	9 // (Reserved)
// Get the enum text
#define XIF_COMMAND_GETENUM	11
// The app-defined command, must greater or equal to than this value
#define XIF_COMMAND_APP	100

/*
 * defined for TxifMessage.status, when issued as a command
 */
#define XIF_RESPOND_BLOCKED	0
#define XIF_RESPOND_UNBLOCKED	1

/*
 * defined for TxifMessage.status, when returned to a command
 */
#define XIF_RETURN_SUCCESS		0 // Return successful
#define XIF_RETURN_NO_SUCH_CMD	1 // TxifMessage.command can not be specified
#define XIF_RETURN_NO_SUCH_OID 2 // TxifMessage.id and TxifMessage.idx error
#define XIF_RETURN_ERROR_VALUE	3 // TxifMessage.values exceed range or wrong value
#define XIF_RETURN_FAIL		4 // Return failure after execute the command
#define XIF_RETURN_TIMMING_OUT	5 // no return in time. NOTE: at this time, the status is set by caller but not by returned message
#endif //// __code_block_xif_TxifMessage
////###############################################################################################
#define __code_block_1 1
#if __code_block_1

// defined for ToidDef.unit/.indexer/.name/.max/.min
#define OID_UNIT_LENGTH 8 // size: 4xn
#define OID_INDEXER_LENGTH 32 // size: 4xn
#define OID_NAME_LENGTH 32 // size: 4xn
#define OID_FLOAT_MAX 1.0e5
#define OID_FLOAT_MIN 1.0e-5

// data type for ToidDef.type
typedef enum ToidType {
	oidType_Int = 0, // type = int
	oidType_Float, // type = float
	oidType_String, // type = char[ToidDef.count][ToidDef.size], or TjoinedStrings
	oidType_Binary, // type = some struct type or (fixed size data array such as char[5])
	oidType_Group, // type = the same as oidType_String
	oidType_EnumText, // type = TenumText
	oidType_DataBlock, // type = TdataBlock, or joined TdataBlock

	/*
	 *  not a enum, just for coding only
	 */
	oidType_Count
} ToidType;

// data type for ToidDef.access
typedef enum ToidAccess {
	oidAccess_Init, // no set yet
	oidAccess_RW, // Read/Write
	oidAccess_RO, // Read only
	oidAccess_None, // not present or not accessible

	/*
	 *  not a enum, just for coding only
	 */
	oidAccess_Count
} ToidAccess;

/*
 * TenumText: the data type has 2 formats: integer and string, like enum type
 * For example:
 *   0 = "Zero", the digital 0 is integer type, and the "Zero" is in string type
 *   1 = "One",
 *   2 = "Two",
 *
 * It act as integer type when in XIM_COMMMAND_GET/XIM_COMMMAND_SET
 * It act as string type when in XIM_COMMMAND_GETS/XIM_COMMMAND_SETS
 */
typedef unsigned char TenumText; // range: from 0 to 0x1F (that is < ' ')


/*
 * TjoinedStrings: multiple strings is joined together, each string is one element.
 * the format of TjoinedStrings:
 *   the split characters is '\0', which split all the strings.
 *   the last elements of string. should not exceed a length of the variable memory.
 *   count of elements: your can defined in the ToidDef.count;
 *
 * which is a virtual struct only for understanding
 * Example:
 *   char texts1[] = {"Astring\0Bstring\0Cstring\0Dstring"};
 *   or
 *   char texts2[30] = {"Astring" "\0" "Bstring" "\0" "Cstring" "\0" "Dstring" };
 *
 *   the texts1 has 4 elements: "Astring", "Bstring",....
 *   the texts2 has 4+ elements: "Astring", "Bstring",....,"Dstring","","",....
 *     the last elements are string "";
 *
 */
typedef struct TjoinedStrings {
	// 1/2/3 represent any size, the last character of s1/s2/s3 is '\0'
	char s1[1];
	char s2[2];
	char s3[3];
} TjoinedStrings;

/*
 * TdataBlock: multiple strings is joined together, each string is one element.
 * the size of the data is variable.
 * the .length would state the size of the data followed, including .length
 * here is a struct template for understanding only
 *
 * note:
 * if .length == 0, the current data is invalid. and no more data behind it.
 * if .length == 1, the real data length is 0.
 *
 */
typedef struct TdataBlock {
	unsigned char length; // range: [0, XIF_MESSAGE_DATA_LENGTH]
	unsigned char bytes[1]; // when 1 may be any in range of [1, XIF_MESSAGE_DATA_LENGTH - 1]
} TdataBlock;

/*
 * ToidStringIndex:
 * The index pointed to oidDefs[], It act as string type
 * case '\0': for null index and not more index followed
 * case x:  index = x - 1
 *
 */
typedef unsigned char ToidStringIndex; //

typedef unsigned char Toid;
typedef unsigned char ToidIndex;
#define OID_ALL_APP '\337'
#define OID_ALL_OID '\337'
#define OID_ALL_INDEX '\337'

typedef struct ToidDef {
	/*
	 * how to find the object and the length of the object data,
	 * when ToidLink::getDirectPtr or setDirectPtr (that is directPtr) is valid:
	 * case oidType_Int(oidType_Float):
	 *     .size = [sizeof(int), n]
	 *     objectPtr = directPtr + index * .size
	 *     objectLength = sizeof(int)
	 *     min/max = the digital range of int;
	 *
	 * case oidType_Float:
	 *     like oidType_Int
	 *
	 * case oidType_Binary:
	 *     .size = [sizeof(struct of Binary), n]
	 *     objectPtr = directPtr + index * .size
	 *     objectLength = sizeof(struct of Binary)
	 *     min/max  = fixed value of length = sizeof(struct of Binary)
	 *
	 * case oidType_String(oidType_Group):
	 *     if .size == 0
	 *         objectPtr = findString(directPtr, index)
	 *     if .size == [1,n]
	 *         objectPtr = directPtr + index * .size
	 *     objectLength = strlen(objectPtr) + 1;
	 *     min/max  = [min length, max length]
	 *
	 * case oidType_EnumText:
	 *     .size = [sizeof(TenumText), n]
	 *     objectPtr = directPtr + index * .size
	 *     lenght1 = sizeof(TenumText), when in digital type
	 *     lenght2 = strlen((*objectPtr->getDirectPtr)(index)) + 1; when in string type
	 *     min/max = the digital range of TenumText; min == 0
	 *     NOTE: In its oidLink, the .getDirectPtr must be set for reading the digital value;
	 *       and the .getFunctionPtr must be set for reading the string value.
	 *
	 * case oidType_DataBlock:
	 *     if .size == 0
	 *         objectPtr = find the TdataBlock (directPtr, index)
	 *     if .size == [1,n]
	 *         objectPtr = directPtr + index * .size
	 *     objectLength = objectPtr->length+1;
	 *     min/max  = [min length, max length]
	 *
	 */

	char unit[OID_UNIT_LENGTH];
	/*
	 * '.','[',']','_' is not allowed in indexer, the '_' is used for web(HTML)
	 */
	char indexer[OID_INDEXER_LENGTH]; // the indexer of oid, unique in app scope, for indexing
	char name[OID_NAME_LENGTH]; // short name of oid, in English only

	/*
	 * limit value:
	 * in case of digital type (int and float etc)
	 *     [OID_FLOAT_MIN, OID_FLOAT_MAX]
	 *     range adjustment for int type: if(value < (int)oidDef.min) ...
	 * else the limit is the length(size) of the data
	 *     [0, XIF_MESSAGE_DATA_LENGTH]
	 */
	float min;
	float max;

	uchar access; // type:ToidAccess
	uchar type; // type:ToidType
	uchar parent; // type: index of oid, use this to get the relations of all oids

	/*
	 * size is used for calculating the address of element[index]
	 * 0: need a method to calculate; used in oid with type oidType_String/oidType_Group/oidType_DataBlock
	 * [1,n]: directly offset of the base address, offset = index * size;
	 */
	uchar size; //
	ushort count; // count of elements

} ToidDef;

#endif //// __code_block_1
////###############################################################################################
#define __code_block_2 1
#if __code_block_2

typedef struct ToidText {
	// Language sensitive text, they would be displayed to users
	char *name;
	char *descr; // description
} ToidText;

/*
 * \brief: a function pointer, to get value of oid
 * \param[in] oid: the index of thisApp_oidDefs[]
 * \param[in] index: the index of the array of thisApp_oidDefs[oid]
 * \param[out] variantPtr: return value
 * \param[out] lengthPtr: length of *variantPtr
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: Before calling this, do check the maximum size would be returned.
 *
 * \note: DO BE UPDATE THE PROPERTIES OF RESPONDING OID, SUCH AS .COUNT, .SIZE!
 */
typedef int (*TgetFunctionPtr)(int oid, int index, TVariant *variantPtr,
		int *lengthPtr);

/*
 * \brief: a function pointer, to set value of oid
 * \param[in] oid: the index of thisApp_oidDefs[]
 * \param[in] index: the index of the array of thisApp_oidDefs[oid]
 * \param[in] variantPtr: input value
 * \param[in] lengthPtr: length of *variantPtr
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: DO BE UPDATE THE PROPERTIES OF RESPONDING OID, SUCH AS .COUNT, .SIZE!
 */
typedef int (*TsetFunctionPtr)(int oid, int index, TVariant *variantPtr,
		int *lengthPtr);

/*
 * \return: (char *) would NOT be a NULL pointer
 */
typedef char *(*TgetEnumPtr)(int value);
/*
 * \param[in] valueInText: would NOT be a NULL pointer
 */
typedef int (*TsetEnumPtr)(char * valueInText);

/*
 * \brief: a function pointer, to execute a customized command
 * \param[in] oid: the index of thisApp_oidDefs[]
 * \param[in] index: the index of the array of thisApp_oidDefs[oid]
 * \param[in/out] prmPtr: input/output value
 * \param[in/out] lengthPtr: length of *prmPtr
 * \return: see XIF_RETURN_* in xif.h
 */
typedef int (*TappDefinedCmd)(int oid, int index, int command, ToidPrm *prmPtr,
		int *lengthPtr);

typedef struct ToidLink {
	// Links(pointers) of get and set operations

	// address(pointer) of oid, access directly
	void *getDirectPtr;
	void *setDirectPtr;

	// function pointer to access oid
	void *getFunctionPtr; // type: TgetFunctionPtr or TgetEnumPtr for TenumText type
	void *setFunctionPtr; // type: TsetFunctionPtr or TsetEnumPtr for TenumText type

	// function pointer to the app defined function
	void *appDefinedCmd; // type: TappDefinedCmd

} ToidLink;

#endif //// __code_block_2
////###############################################################################################
#define __code_block_3 1
#if __code_block_3
char *getEnumText_ToidType(int value);
char *getEnumText_ToidAccess(int value);

#endif //// __code_block_3
#endif // _XIF_H_
