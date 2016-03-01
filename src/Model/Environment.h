/* 
 * File:   Environment.h
 * Author: mb425
 *
 * Created on 26 November 2015, 13:32
 */


#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H
#include <Logger.h>
#include <NcGridCell.h>
#include <FileReader.h>
#include <FileWriter.h>
#include <Convertor.h>
#include <Constants.h>
#include <DataGrid.h>
#include<iostream>
#include <string>
#include <map>
class GridCell;
using namespace std;
typedef vector <vector<double> > LayerData;
////////////////////////////////////////////////////////////////////////////////
class layer{
public:
    virtual ~layer(){;}
    virtual vector<double>& operator[](int i)=0;
    virtual void setTime(int tm)=0;
};
////////////////////////////////////////////////////////////////////////////////
class layer0 : public layer {
    LayerData data;
public:
    layer0(int sx, int sy) {
        data.resize(sx);
        for (unsigned m = 0; m < sx; m++)data[m].resize(sy);
    }
    //--------------------------------------------------------------------------
    ~layer0(){
       for (unsigned m = 0; m < data.size(); m++)data[m].clear(); 
    }
    //--------------------------------------------------------------------------
    vector<double>& operator[](int i) {
        return data[i];
    }
    //--------------------------------------------------------------------------
    void setTime(int tm){;}
};
////////////////////////////////////////////////////////////////////////////////
class layerT : public layer {
    int t;
    vector<LayerData> data;
public:
    layerT(int q, int sx, int sy) : t(0) {
        data.resize(q);
        for (unsigned u = 0; u < q; u++) {
            data[u].resize(sx);
            for (unsigned i = 0; i < sx; i++) {
                data[u][i].resize(sy);
            }
        }
    }
    //--------------------------------------------------------------------------
    ~layerT(){
       for (unsigned m = 0; m < data.size(); m++){
           for (unsigned n=0;n<data[m].size();n++)data[m][n].clear();   
           data[m].clear();
       }
    }
    //--------------------------------------------------------------------------
    vector<double>& operator[](int i) {
        return data[t][i];
    }
    //--------------------------------------------------------------------------
    void setTime(int tm){t=tm;}
};
////////////////////////////////////////////////////////////////////////////////
class Environment {

    static Environment* Instance;
    static map<string,layer*> Layers;
    Environment();


    void addLayer(string);
    void addLayerT(string);
void setUVel();
void setVVel();
void setTemperature();
void setDiurnalTemperatureRange();
void setPrecipitation() ;
void setNPP();
void setRealm();
void setOrganicPool();
void setRespiratoryCO2Pool();
void setAVGSDTemp();
void setNPPSeasonality();
void setBreeding();
void setFrostandFire();
void setHANPP();
public:
    static const double MissingValue;
    static Environment* Get();
    static double Get(string s, GridCell& gcl, int tm);
    static double& Get(string s, GridCell& gcl);
    static double& Get(string s, int,int);

    static void update(int);
};

#endif	/* ENVIRONMENT_H */


