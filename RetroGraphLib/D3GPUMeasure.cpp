#include "stdafx.h"

#include "D3GPUMeasure.h"

// #include "d3dkmt.h"
// #include "phbasesup.h"
#include <iostream>

#include "utils.h"

namespace rg {

//typedef struct _RTL_BITMAP {
//    ULONG SizeOfBitMap;
//    PULONG Buffer;
//} RTL_BITMAP, *PRTL_BITMAP;
//
//
//typedef struct _ETP_GPU_ADAPTER {
//    LUID AdapterLuid;
//    ULONG SegmentCount;
//    ULONG NodeCount;
//    ULONG FirstNodeIndex;
//
//    PPH_STRING DeviceInterface;
//    PPH_STRING Description;
//    PPH_LIST NodeNameList;
//
//    RTL_BITMAP ApertureBitMap;
//    ULONG ApertureBitMapBuffer[1];
//} ETP_GPU_ADAPTER, *PETP_GPU_ADAPTER;
//

D3GPUMeasure::D3GPUMeasure() :
    Measure{ 1U } {
}


D3GPUMeasure::~D3GPUMeasure() {
}

void rg::D3GPUMeasure::update(uint32_t) {
    // PETP_GPU_ADAPTER gpuAdapter;
    //D3DKMT_QUERYSTATISTICS queryStatistics;
    //ULONG64 totalRunningTime;

    //if (!Block->ProcessItem->QueryHandle)
    //    return;

    //totalRunningTime = 0;

    //for (ULONG i = 0; i < EtpGpuAdapterList->Count; i++) {
    //    gpuAdapter = EtpGpuAdapterList->Items[i];

    //for (ULONG j = 0; j < gpuAdapter->NodeCount; j++) {
        //memset(&queryStatistics, 0, sizeof(D3DKMT_QUERYSTATISTICS));
        //queryStatistics.Type = D3DKMT_QUERYSTATISTICS_PROCESS_NODE;
        //queryStatistics.AdapterLuid = gpuAdapter->AdapterLuid;
        //queryStatistics.ProcessHandle = Block->ProcessItem->QueryHandle;
        //queryStatistics.QueryProcessNode.NodeId = j;

        //if (NT_SUCCESS(D3DKMTQueryStatistics(&queryStatistics))) {
        //    //ULONG64 runningTime;
        //    //runningTime = queryStatistics.QueryResult.ProcessNodeInformation.RunningTime.QuadPart;
        //    //PhUpdateDelta(&Block->GpuTotalRunningTimeDelta[j], runningTime);

        //    totalRunningTime += queryStatistics.QueryResult.ProcessNodeInformation.RunningTime.QuadPart;
        ////}
    //}
    //}

    //PhUpdateDelta(&Block->GpuRunningTimeDelta, totalRunningTime);
}

bool D3GPUMeasure::shouldUpdate(uint32_t ticks) const {
    return ticksMatchRate(ticks, m_updateRates.front());
}

}
