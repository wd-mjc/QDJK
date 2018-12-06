/*
@file     Common.h
@brief    通用功能宏定义
@author   mjc
@version  1.00
@create date     2016-05-17 10:00
@note
*/
#ifndef		_COMMON_H
#define		_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


#define STM32   //当前环境为STM32

#if defined (STM8)//定义数据类型

#ifndef UINT8
    #define UINT8 unsigned char
#endif	
#ifndef SINT8
    #define SINT8 signed char
#endif	
#ifndef UINT16
    #define UINT16 unsigned int
#endif	
#ifndef SINT16
    #define SINT16 signed int
#endif	
#ifndef UINT32
    #define UINT32 unsigned long
#endif	
#ifndef SINT32
    #define SINT32 signed long
#endif	
	
#elif defined (STM32)

#ifndef UINT8
    #define UINT8 unsigned char
#endif	
#ifndef SINT8
    #define SINT8 signed char
#endif
#ifndef UINT16
    #define UINT16 unsigned short
#endif	
#ifndef SINT16
    #define SINT16 signed short
#endif
#ifndef UINT32
    #define UINT32 unsigned int
#endif
#ifndef SINT32
    #define SINT32 signed int
#endif
#ifndef UINT64
	#define UINT64 unsigned long
#endif
#ifndef SINT64
	#define SINT64 signed long
#endif

#endif


//大小端转换
#define Tranverse16(X)  ((((UINT16)(X) & 0xff00) >> 8) |(((UINT16)(X) & 0x00ff) << 8))
#define Tranverse32(X)  ((((UINT32)(X) & 0xff000000) >> 24) | (((UINT32)(X) & 0x00ff0000) >> 8) \
                      | (((UINT32)(X) & 0x0000ff00) << 8) | (((UINT32)(X) & 0x000000ff) << 24))




/************************************************************************/
/*			 BIT位定义                                                  */
/************************************************************************/
#define DB0                             (0x01)
#define DB1                             (0x02)
#define DB2                             (0x04)
#define DB3                             (0x08)
#define DB4                             (0x10)
#define DB5                             (0x20)
#define DB6                             (0x40)
#define DB7                             (0x80)
#define DB8                             (0x100)
#define DB9                             (0x200)
#define DB10                            (0x400)
#define DB11                            (0x800)
#define DB12                            (0x1000)
#define DB13                            (0x2000)
#define DB14                            (0x4000)
#define DB15                            (0x8000)
#define DB16                            (0x10000)
#define DB17                            (0x20000)
#define DB18                            (0x40000)
#define DB19                            (0x80000)
#define DB20                            (0x100000)
#define DB21                            (0x200000)
#define DB22                            (0x400000)
#define DB23                            (0x800000)
#define DB24                            (0x1000000)
#define DB25                            (0x2000000)
#define DB26                            (0x4000000)
#define DB27                            (0x8000000)
#define DB28                            (0x10000000)
#define DB29                            (0x20000000)
#define DB30                            (0x40000000)
#define DB31                            (0x80000000)

/************************************************************************/
/*			 小端模式字节顺序定义                                       */
/************************************************************************/
#define  CN_BYTE_LO                     (0)                     // 低字节
#define  CN_BYTE_HI                     (1)                     // 高字节

#define  CN_WORD_LO                     (0)                     // 低字
#define  CN_WORD_HI                     (1)                     // 高字

#define  CN_DWB4_LL                     (0)                     // 最低字节
#define  CN_DWB4_LH                     (1)                     // 次低字节
#define  CN_DWB4_HL                     (2)                     // 次高字节
#define  CN_DWB4_HH                     (3)                     // 最高字节

#define  CN_2DWB4_LL                    (0)                     // 最低字节
#define  CN_2DWB4_LH                    (1)                     // 次低字节
#define  CN_2DWB4_HL                    (2)                     // 次高字节
#define  CN_2DWB4_HH                    (3)                     // 最高字节
#define  CN_1DWB4_LL                    (4)                     // 最低字节
#define  CN_1DWB4_LH                    (5)                     // 次低字节
#define  CN_1DWB4_HL                    (6)                     // 次高字节
#define  CN_1DWB4_HH                    (7)                     // 最高字节
					  

#define MD_CHAR                char
#define MD_SHORT               signed short
#define MD_INT                 signed int
#define MD_LONG                signed long int
#define MD_BYTE                unsigned char
#define MD_WORD                unsigned short
#define MD_DWORD               unsigned int
#define MD_DOUBLE              double
#define MD_FLOAT               float
#define MD_BOOLEAN             unsigned char


#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef OFF
#define OFF     0
#endif

#ifndef ON
#define ON      1
#endif

#ifndef NO
#define NO      0
#endif

#ifndef YES
#define YES     1
#endif

#ifndef OK
#define OK		0
#endif

#define M_CreatFile( filename )         creat( filename, O_WRONLY )
#define M_OpenFileR( filename )         open( filename, O_RDONLY, 0777 )
#define M_OpenFileW( filename )         open( filename, (O_WRONLY | O_CREAT), 0777 )
#define M_OpenFileRW( filename )        open( filename, O_RDWR, 0 )
#define M_ReadFile( fd, buf, nbytes )   read( fd, buf, nbytes )
#define M_WriteFile( fd, buf, nbytes )  write( fd, buf, nbytes )
#define M_CloseFile( fd )               close( fd )
#define M_RemoveFile( filename )        remove( filename )
#define M_CreatDir( dirname )           open( dirname, (O_WRONLY | O_CREAT), (FSTAT_DIR | 0777) )
#define M_OpenDir( dirname )            opendir( dirname )
#define M_ReadDir( pdir )               readdir( pdir )
#define M_CloseDir( pdir )              closedir( pdir )


/*================================================================================*/
// 宏定义
/*================================================================================*/
#define M_GetByteLL( x )                (MD_BYTE)( (x) & 0xFF )                        // 获取DMD_WORD的LL
#define M_GetByteLH( x )                (MD_BYTE)( ( (x) >> 8 ) & 0xFF )               // 获取DMD_WORD的LH
#define M_GetByteHL( x )                (MD_BYTE)( ( (x) >>16 ) & 0xFF )               // 获取DMD_WORD的HL
#define M_GetByteHH( x )                (MD_BYTE)( ( (x) >>24 ) & 0xFF )               // 获取DMD_WORD的LH

#define M_Get_WORDLO( x )                (MD_WORD)( (x) & 0xFFFF )                      // 获取DMD_WORD低16位
#define M_Get_WORDHI( x )                (MD_WORD)( ( (x) >>16 ) & 0xFFFF )             // 获取DMD_WORD高6位
#define M_Make_WORD(l, h)                ((((MD_WORD)(h)) << 8 ) | ((MD_WORD)(l) & 0xFF) ) // 两个MD_BYTE合成为MD_WORD
#define M_Make_DWORD(l, h)               ((l)&0xFFFF) | (((h)&0xFFFF)<<16)           // 两个MD_WORD合成为DMD_WORD

#define M_HexToBcd( x )                 ((((x) / 10) <<4) + ((x) % 10) )            // 将16进制数转换为BCD码
#define M_BcdToHex( x )                 ((((x) & 0xF0) >>4) * 10 + ((x) & 0x0F) )   // 将BCD码转换为16进制数
#define M_SetBit( x, n)                 ((x) = (x) |   (1<<(n))  )                  // 置位
#define M_SETBIT( x, n)                 ((x) = (x | n))
#define M_CLRBIT( x, n)                 ((x) = (x & (~n)))
#define M_CHKBIT( x, n)                 ( x & n)

#define M_ClrBit( x, n)                 ((x) = (x) & (~(1<<(n))) )                  // 清位
#define M_GetBit( x, n)                 ((x) &  (1<<(n)) )                          // 获取位状态，注意：不赋值

#define M_AddrAlign4( Addr )            (((Addr) & 0xFFFFFFFC)+4 )                  // 强制地址4字节对齐
#define M_Swap( a, b, tmp )             do{(tmp)=(a); (a)=(b); (b)=tmp;}while(0)
#define M_MAX( x, y )                   (((x)>(y)) ? (x) : (y) )                    // 找最大值
#define M_MIN( x, y )                   (((x)<(y)) ? (x) : (y) )                    // 找最小值


#ifdef		SYSTEM_WINDOWS
	#define		FD_PRINTF(fd,cStr)					(fputs(cStr,fd))
#else
    #define		FD_PRINTF(fd,cStr)					(fprintf( fd, "%s", cStr))
#endif

// ============================================================================
/**
   @brief   宏功能,保存一个字节
   @param   输入参数,保存基址 需保存的字和字节联合体数据
   @param   输出参数,编码后保存地址新偏移
   @return  无
*/
// ============================================================================
#define M_SAVE_BYTE( pbySaveBuf, byVal )                    \
{                                                           \
    (*(pbySaveBuf)++) = (byVal);                            \
}
// ============================================================================
/**
   @brief   宏功能,保存一个字
   @param   输入参数,保存基址 需保存的字和字节联合体数据
   @param   输出参数,编码后保存地址新偏移
   @return  无
*/
// ============================================================================
#define M_SAVE_MD_WORD( pbySaveBuf, wVal, tWB2 )               \
{                                                           \
    tWB2.wMD_WORD = wVal;                                      \
    (*(pbySaveBuf)++) = (tWB2).cByte[CN_BYTE_LO];           \
    (*(pbySaveBuf)++) = (tWB2).cByte[CN_BYTE_HI];           \
}
// ============================================================================
/**
   @brief   宏功能,保存一个双字
   @param   输入参数,保存基址 需保存的双字、字和字节联合体数据
   @param   输出参数,编码后保存地址新偏移
   @return  无
*/
// ============================================================================
#define M_SAVE_DMD_WORD( pbySaveBuf, dwVal, tWB4 )             \
{                                                           \
    tWB4.dwDMD_WORD = dwVal;                                   \
    (*(pbySaveBuf)++) = (tWB4).cByte[CN_DWB4_LL];           \
    (*(pbySaveBuf)++) = (tWB4).cByte[CN_DWB4_LH];           \
    (*(pbySaveBuf)++) = (tWB4).cByte[CN_DWB4_HL];           \
    (*(pbySaveBuf)++) = (tWB4).cByte[CN_DWB4_HH];           \
}
// ============================================================================
/**
   @brief   宏功能,读取一个字节
   @param   输入参数,读取基址 需读取的字和字节联合体数据
   @param   输出参数,解码后读取地址新偏移
   @return  无
*/
// ============================================================================
#define M_READ_BYTE( pbyReadBuf, byVal )                    \
{                                                           \
    (byVal) = (*(pbyReadBuf)++);                            \
}
// ============================================================================
/**
   @brief   宏功能,读取一个字
   @param   输入参数,读取基址 需读取的字和字节联合体数据
   @param   输出参数,解码后读取地址新偏移
   @return  无
*/
// ============================================================================
#define M_READ_WORD( pbyReadBuf, wVal, tWB2 )              \
{                                                           \
    (tWB2).cByte[CN_BYTE_LO] = (*(pbyReadBuf)++);           \
    (tWB2).cByte[CN_BYTE_HI] = (*(pbyReadBuf)++);           \
    wVal = tWB2.wWord;                                      \
}
// ============================================================================
/**
   @brief   宏功能,读取一个双字
   @param   输入参数,读取基址 需读取的双字、字和字节联合体数据
   @param   输出参数,解码后读取地址新偏移
   @return  无
*/
// ============================================================================
#define M_READ_DWORD( pbyReadBuf, dwVal, tWB4 )             \
{                                                           \
    (tWB4).cByte[CN_DWB4_LL] = (*(pbyReadBuf)++);           \
    (tWB4).cByte[CN_DWB4_LH] = (*(pbyReadBuf)++);           \
    (tWB4).cByte[CN_DWB4_HL] = (*(pbyReadBuf)++);           \
    (tWB4).cByte[CN_DWB4_HH] = (*(pbyReadBuf)++);           \
    dwVal = tWB4.dwDMD_WORD;                                   \
}
// ============================================================================
/**
   @brief   宏功能,按字节转存
   @param   输入参数,源基址,目标基址, 需转移字节个数
   @param   输出参数,无
   @return  无
*/
// ============================================================================
#define M_COPY_BYTE( pbySrcBuf, pbyDstBuf,wNum )            \
{                                                           \
    memcpy(pbyDstBuf, pbySrcBuf, wNum);                     \
    pbyDstBuf += wNum;                                      \
    pbySrcBuf += wNum;                                      \
}
// ============================================================================
/**
   @brief   宏功能,检查个数正反码是否一致
   @param   输入参数,正码,反码, 正反码一致标志
   @param   输出参数,无
   @return  无
*/
// ============================================================================
#define M_CHK_NUM( wNum, wNumNot, bFlag )                   \
{                                                           \
    MD_WORD                wTemp;                              \
    wTemp   = ( MD_WORD )~wNum;                                \
    if( wTemp != wNumNot )                                  \
    {                                                       \
        bFlag   = FALSE;                                    \
    }else                                                   \
    {                                                       \
        bFlag   = TRUE;                                     \
    }                                                       \
}
// ============================================================================
/**
   @brief   宏功能,tag检查
   @param   输入参数,获取的tag,目标tag, 是否一致标志
   @param   输出参数,无
   @return  无
*/
// ============================================================================
#define M_CHK_TAG( wSrcTag, wDstTag, bFlag )                \
{                                                           \
    if( wSrcTag != wDstTag )                                \
    {                                                       \
        bFlag   = FALSE;                                    \
    }else                                                   \
    {                                                       \
        bFlag   = TRUE;                                     \
    }                                                       \
}


extern UINT16 CRC16(UINT16 *Buf, UINT16 Len);
extern UINT8 CRC8(UINT8 *p, UINT16 counter);
extern UINT8 CalcXor(UINT8* data, UINT16 len);
extern UINT16 removemultip(UINT16 *a, UINT16 n);

#ifdef __cplusplus
}
#endif

#endif
