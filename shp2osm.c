
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shapefil.h"
#include "quadtree.h"

void key_free(void* key) {
    free(key);
    key = NULL;
}

int main( int argc, char ** argv )
{
    SHPHandle hWaySHP;
    DBFHandle hWayDBF;
    int       nShapeType, nWays, dbfCount, nWidth, nDecimals;
    double    adfMinBound[4], adfMaxBound[4];
    char      szTitle[12];
    int       i = 0;
    FILE*     fp_point = NULL;
    FILE*     fp_way = NULL;

    quadtree_t* tree = NULL;
    point_t*    point = NULL;
    int         point_num = 0;
    int*        point_id = NULL;
    double      zero = 0.000000001;

    char*       inputfile = NULL;
    char*       outputfile = NULL;
    char*       tempfile = "temp";

/* -------------------------------------------------------------------- */
/*      Display a usage message.                                        */
/* -------------------------------------------------------------------- */
    if( argc < 2 )
    {
        printf( "shp2osm: convert shapefile to osm\n" );
        printf( "Usage: \n" );
        printf( "shp2osm way_shp_file \n" );
        exit( 1 );
    }
    inputfile = argv[1];
    outputfile = str_replace(inputfile, ".shp", ".osm");
    if (!strstr(outputfile, ".osm")) {
        strcat(outputfile, ".osm");
    }

/* -------------------------------------------------------------------- */
/*      Open the passed shapefile.                                */
/* -------------------------------------------------------------------- */

    hWaySHP = SHPOpen( inputfile, "rb" );

    if( hWaySHP == NULL ) {
        printf( "Unable to open:%s\n", argv[1] );
        exit( 1 );
    }

    hWayDBF = DBFOpen( inputfile, "rb" );
    if( hWayDBF == NULL ) {
        printf( "DBFOpen(%s,\"r\") failed.\n", argv[1] );
        exit( 2 );
    }

    SHPGetInfo( hWaySHP, &nWays, &nShapeType, adfMinBound, adfMaxBound );
    dbfCount = DBFGetRecordCount(hWayDBF);
    if(dbfCount != nWays) {
        printf("dbf number error \n");
        exit(1);
    }

/* -------------------------------------------------------------------- */
/*	foreach way.                                                        */
/* -------------------------------------------------------------------- */

    fp_way = fopen(tempfile, "w");
    fp_point = fopen(outputfile, "w");
    fprintf(fp_point, "<?xml version='1.0' encoding='UTF-8'?>\n");
    fprintf(fp_point, "<osm version=\"0.6\" generator=\"shp2osm\">\n");

    tree = quadtree_new(adfMinBound[0]-zero, adfMinBound[1]-zero, adfMaxBound[0]+zero, adfMaxBound[1]+zero);
    tree->key_free = key_free;

    for( i = 0; i < nWays; i++ )
    {
        int j;
        char *  temp_record = NULL;
        SHPObject *psShape;
        psShape = SHPReadObject( hWaySHP, i );
        if( psShape == NULL ) {
            fprintf( stderr, "Unable to read shape %d, terminating object reading.\n", i );
            break;
        }

        fprintf(fp_way, "  <way id=\"%d\">\n", i);
        for( j = 0; j < psShape->nVertices; j++ ) {
            point_id = malloc(sizeof(int));
            *point_id = point_num;
            point = quadtree_insert(tree, psShape->padfX[j], psShape->padfY[j], (void*)point_id, false);
            if (!point) {
                printf("lat=\"%f\" lon=\"%f\" insert error \n", temp_y*zero, temp_x*zero);
                continue;
            }
            if(*(int*)point->key == point_num) {
                fprintf(fp_point, "  <node id=\"%d\" lat=\"%f\" lon=\"%f\"/>\n", *(int*)point->key, point->y, point->x);
                point_num++;
            }
            fprintf(fp_way, "    <nd ref=\"%d\"/>\n", *(int*)point->key);
        }
        SHPDestroyObject( psShape );

        // write attribute
        for( j = 0; j < DBFGetFieldCount(hWayDBF); j++ ) {
            DBFFieldType    eType;
            eType = DBFGetFieldInfo( hWayDBF, j, szTitle, &nWidth, &nDecimals );
            if(eType == FTString) {
                fprintf(fp_way, "    <tag k=\"%s\" v=\"%s\"/>\n", szTitle, DBFReadStringAttribute( hWayDBF, i, j));
            } else if (eType == FTInteger) {
                fprintf(fp_way, "    <tag k=\"%s\" v=\"%d\"/>\n", szTitle, DBFReadIntegerAttribute( hWayDBF, i, j));
            } else if (eType == FTDouble) {
                fprintf(fp_way, "    <tag k=\"%s\" v=\"%lf\"/>\n", szTitle, DBFReadDoubleAttribute( hWayDBF, i, j));
            }
        }
        fprintf(fp_way, "  </way>\n");

    }

    quadtree_free(tree);

    SHPClose( hWaySHP );
    DBFClose( hWayDBF );

    fclose(fp_way);
    fp_way = NULL;

    if(1) {
        char c;
        fp_way = fopen(tempfile, "r");
        while((c=fgetc(fp_way))!=EOF)
        {
            fputc(c,fp_point);
        }
        fclose(fp_way);
        fp_way = NULL;
        remove(tempfile);
    }
    fprintf(fp_point, "</osm>\n");

    fclose(fp_point);
    fp_point = NULL;

    if (outputfile) {
        free(outputfile);
    }

    return 0;
}
