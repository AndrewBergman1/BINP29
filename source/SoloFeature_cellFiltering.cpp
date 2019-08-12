#include "SoloFeature.h"
#include "streamFuns.h"
#include "TimeFunctions.h"
#include "serviceFuns.cpp"
#include "SequenceFuns.h"
#include "ErrorWarning.h"

void SoloFeature::cellFiltering()
{    
    if (pSolo.cellFilter.type[0]=="None")
        return;
    
    //sort nUperCB
    vector<uint32> nUMIperCBsorted=nUMIperCB;
    qsort(nUMIperCBsorted.data(), nCB, sizeof(uint32), funCompareNumbersReverse<uint32>); //sort by gene number

    uint32 nUMImax=0, nUMImin=0;
    if (pSolo.cellFilter.type[0]=="CellRanger2.2") {
        //find robust max
        nUMImax = nUMIperCBsorted[min(nCB-1,pSolo.cellFilter.cr2maxCellInd)];//robust estimate of the max UMI
        nUMImin = int(nUMImax/pSolo.cellFilter.cr2maxMinRatio+0.5);
    } else if (pSolo.cellFilter.type[0]=="TopCells") {
        nUMImin = nUMIperCB[max(nCB-1,pSolo.cellFilter.topCells)];
    };

    cellFilterVec.resize(nCB,false);
    for (uint32 icb=0; icb<nCB; icb++) {
        if (nUMIperCB[icb]>=nUMImin) {
            cellFilterVec[icb]=true;
            
            filteredCells.nCells++;

            filteredCells.nUMIinCells += nUMIperCB[icb];
            
            filteredCells.nGeneInCells += nGenePerCB[icb];
            filteredCells.nGenePerCell.push_back(nGenePerCB[icb]);
            
            filteredCells.nReadInCells += nReadPerCB[icb];
            filteredCells.nReadPerCell.push_back(nReadPerCB[icb]);
        };
    };
    
    filteredCells.meanUMIperCell = filteredCells.nUMIinCells / filteredCells.nCells;
    filteredCells.meanReadPerCell = filteredCells.nReadInCells / filteredCells.nCells;
    filteredCells.meanGenePerCell = filteredCells.nGeneInCells / filteredCells.nCells;
    
    sort(filteredCells.nReadPerCell.begin(), filteredCells.nReadPerCell.end());
    sort(filteredCells.nGenePerCell.begin(), filteredCells.nGenePerCell.end());

    filteredCells.medianUMIperCell = nUMIperCBsorted[filteredCells.nCells/2];
    filteredCells.medianGenePerCell = filteredCells.nGenePerCell[filteredCells.nCells/2];
    filteredCells.medianReadPerCell = filteredCells.nReadPerCell[filteredCells.nCells/2];
};
