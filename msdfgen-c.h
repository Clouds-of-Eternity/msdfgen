#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct 
{
    void *data;
    int width;
    int height;
} MsdfBitmapRef;

typedef struct MsdfShapeImpl *MsdfShape;
typedef struct MsdfGeneratorConfigImpl *MsdfGeneratorConfig;

typedef struct { double x, y; } MsdfPoint;
typedef struct { uint_least16_t begin, end; } MsdfLine;
typedef struct { uint_least16_t begin, end, ctrl; } MsdfCurve;

typedef struct
{
	MsdfPoint *points;
	MsdfCurve *curves;
	MsdfLine  *lines;
	uint_least16_t numPoints;
	uint_least16_t capPoints;
	uint_least16_t numCurves;
	uint_least16_t capCurves;
	uint_least16_t numLines;
	uint_least16_t capLines;
} MsdfShapeCreateInfo;

typedef struct
{
    void *memory;
    uint32_t width;
    uint32_t height;
} MsdfBitmapCreateInfo;

typedef struct
{
    double translateX;
    double translateY;
    double scaleX;
    double scaleY;
} MsdfTransformation;

MsdfShape MsdfCreateShape(MsdfShapeCreateInfo *createInfo);
void MsdfFreeShape(MsdfShape shape);
void MsdfEdgeColoringSimple(MsdfShape shape);

void MsdfGenerate(MsdfBitmapRef *output, MsdfShape shape, MsdfTransformation transform);

#ifdef __cplusplus
}
#endif