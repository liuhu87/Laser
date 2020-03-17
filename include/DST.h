#ifndef __DST_h__
#define __DST_h__
#define Tmaxentry 1300
struct Data{

   ////////////////////////////////////////////////////////////////
   // The ordering is not random. Unfortunately, due to alignment
   // the variables should be ordered from longer to smaller bit size
   //////////////////////////////////////////////////////////////////

   // $BEGIN -- Mark to start translation

   Short_t iTel;
   Double_t T1MJD;
   Double_t mAdcH_BC[1024];
   Double_t mAdcH_SLC_BGM[1024];
   Double_t mAdcL_SLC_BGM[1024];
   Double_t mBaseH_C[1024];
   Double_t Temp_SiPM_C[1024];
   Double_t SiPM_Temp_Factor[1024];
   Double_t LED_DR_Temp_Factor[1024];
   Double_t table_Factor[1024];
   Double_t BGM_Correct_Factor[1024];
   Double_t T1HV;
   Double_t T1Door1Deg;
   Double_t T1Door2Deg;
   Double_t T1DledDrDStem;
   Double_t T1HVCurrent;

   //$END -- Mark to stop translation

};
#endif
