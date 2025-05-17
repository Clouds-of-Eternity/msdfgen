#include "msdfgen-c.h"
#include "msdfgen.h"
#include <stdlib.h>
#include <assert.h>

typedef msdfgen::BitmapRef<float, 3> Bitmap3f;

MsdfShape MsdfCreateShape(MsdfShapeCreateInfo *createInfo)
{
    msdfgen::Shape *shape = new msdfgen::Shape();


    uint8_t *pointState = (uint8_t *)calloc(createInfo->numPoints, sizeof(uint8_t));
    //0: is default
    //1: is control point
    //2: has been iterated
    uint32_t *nextPoint = (uint32_t *)malloc(createInfo->numPoints * sizeof(uint32_t));
    for (uint32_t i = 0; i < createInfo->numCurves; i++)
    {
        pointState[createInfo->curves[i].ctrl] = 1;
        nextPoint[createInfo->curves[i].begin] = createInfo->curves[i].ctrl;
        nextPoint[createInfo->curves[i].ctrl] = createInfo->curves[i].end;
    }
    for (uint32_t i = 0; i < createInfo->numLines; i++)
    {
        nextPoint[createInfo->lines[i].begin] = createInfo->lines[i].end;
    }

    uint32_t iterated = 0;
    msdfgen::Contour contour = msdfgen::Contour();
    uint32_t index = 0;
    while (iterated < createInfo->numPoints)
    {
        pointState[index] = 2;

        MsdfPoint begin = createInfo->points[index];
        MsdfPoint end = createInfo->points[nextPoint[index]];
        if (pointState[nextPoint[index]] == 1)
        {
            MsdfPoint control = end;
            end = createInfo->points[nextPoint[nextPoint[index]]];
            iterated++; //we never iterate on control points so just increase the thing
            index = nextPoint[nextPoint[index]];

            contour.addEdge(msdfgen::EdgeHolder(msdfgen::Point2(begin.x, begin.y), msdfgen::Point2(control.x, control.y), msdfgen::Point2(end.x, end.y)));
        }
        else
        {
            index = nextPoint[index];

            contour.addEdge(msdfgen::EdgeHolder(msdfgen::Point2(begin.x, begin.y), msdfgen::Point2(end.x, end.y)));
        }
        if (pointState[index] == 2) //end shape
        {
            assert(contour.edges.size() >= 2);
            // for (uint32_t i = 0; i < contour.edges.size(); i++)
            // {
            //     msdfgen::LinearSegment * segment = (msdfgen::LinearSegment *)contour.edges[i].edgeSegment;

            //     printf("e%u: %f, %f\n", i, segment->p[0].x, segment->p[0].y);
            // }
            // printf(" - %u, winding: %i\n", contour.edges.size(), contour.winding());
            
            shape->addContour(contour);
            contour = msdfgen::Contour();

            //find next index
            bool foundNext = false;
            for (uint32_t j = 0; j < createInfo->numPoints; j++)
            {
                if (pointState[j] == 0)
                {
                    foundNext = true;
                    index = j;
                    break;
                }
            }
            if (!foundNext)
            {
                break;
            }
        }
        iterated++;
    }

    free(nextPoint);
    free(pointState);

    return (MsdfShape)shape;
}
void MsdfFreeShape(MsdfShape shape)
{
    delete shape;
}
void MsdfEdgeColoringSimple(MsdfShape shape)
{
    msdfgen::Shape *shapePtr = (msdfgen::Shape *)shape;
    msdfgen::edgeColoringSimple(*shapePtr, 3.0);
}

void MsdfGenerate(MsdfBitmapRef *output, MsdfShape shape, MsdfTransformation transform)
{
    Bitmap3f bitmapRef = Bitmap3f((float*)output->data, output->width, output->height);

    msdfgen::Vector2 scale = msdfgen::Vector2(transform.scaleX, transform.scaleY);//transform.scaleX, transform.scaleY);
    msdfgen::Vector2 translate = msdfgen::Vector2((transform.translateX + 1.0f) / transform.scaleX, (transform.translateY + 2.0f) / transform.scaleY);      // transform.translateX, transform.translateY);
    msdfgen::Projection projection = msdfgen::Projection(scale, translate);
    msdfgen::DistanceMapping distanceMapping = msdfgen::DistanceMapping(2.0, 0.0);
    msdfgen::SDFTransformation transformation = msdfgen::SDFTransformation(projection, distanceMapping);

    msdfgen::MSDFGeneratorConfig defaultConfig = msdfgen::MSDFGeneratorConfig();
    msdfgen::generateMSDF(bitmapRef, *(msdfgen::Shape *)shape, transformation, defaultConfig);
}