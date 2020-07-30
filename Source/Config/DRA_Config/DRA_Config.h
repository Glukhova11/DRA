#ifndef __DRA__CONFIG__DRA_CONFIG_H__
#define __DRA__CONFIG__DRA_CONFIG_H__

#include <ATE/Types/Array.h>
#include <ATE/Utils/Reloadable.h>

#include <TCM/Config/DRA_Config/DRA_ConfigDynaData.h>

#include <memory>



class Tm_DRA_Config :
	public Tm_PrimReloadableDI<Tm_DRA_ConfigDynaData>,
	public Tm_OnceInstantiated<Tm_DRA_Config>
{
public:
	Tm_DRA_Config(Tm_PrimPMM* pMM);
	~Tm_DRA_Config();

private:
	Tm_DRA_ConfigDynaData_Ptr m_ptr;
        Tm_PrimPMM* const m_pMM;
        
public:
	virtual bool SetData(std::string& sLoadResultInfo, Tm_Loadable_Ptr Data_ptr) override;
	virtual bool IsPrimResponseSync() override;
	inline Tm_PrimPMM* GetPMM() {return m_pMM;};

        //[General]
	const int Handlers() const;
	const int NormalBusyHandlers() const;
	const int CriticalBusyHandlers() const;
	const int NormalQueueSize() const;
	const int CriticalQueueSize() const;
	const int Tx_Timeout() const;
	const int GetCoreCount() const;
        

	//[TrafficManager]
	const int StatCollectionInterval() const;

	//[DataModel]
	const int SessionExpireTime() const;
	const int SyncTimer() const;


	//[Errors]
	int GetDiamTooBusyCount();
		
};

#endif //__DRA__CONFIG__DRA_CONFIG_H__
