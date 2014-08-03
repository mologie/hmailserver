// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "FetchAccountUIDList.h"
#include "../Common/BO/FetchAccountUID.h"
#include "../Common/Persistence/PersistentFetchAccount.h"
#include "../Common/Util/Time.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   FetchAccountUIDList::FetchAccountUIDList(void)
   {
   }

   FetchAccountUIDList::~FetchAccountUIDList(void)
   {
   }
   
   void 
   FetchAccountUIDList::Refresh(__int64 iFAID)
   {
      faid_ = iFAID;

      SQLCommand command("select * from hm_fetchaccounts_uids where uidfaid = @UIDFAID");
      command.AddParameter("@UIDFAID", faid_);

      shared_ptr<DALRecordset> pUIDRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pUIDRS)
         return;

      while (!pUIDRS->IsEOF())
      {
         int iUIDID = pUIDRS->GetLongValue("uidid");
         String sUIDValue = pUIDRS->GetStringValue("uidvalue");
         String sUIDTime = pUIDRS->GetStringValue("uidtime");

         shared_ptr<FetchAccountUID> pUID = shared_ptr<FetchAccountUID>(new FetchAccountUID(iUIDID, faid_, sUIDValue, sUIDTime));

         fetched_uids_.insert(std::make_pair(sUIDValue, pUID));

         pUIDRS->MoveNext();     
      }

   }

   void
   FetchAccountUIDList::AddUID(const String &sUIDValue)
   {
      // Check so that it does not already exist first.
      if (IsUIDInList(sUIDValue))
         return;

      // Save current time as create time
      String sCreateTime = Time::GetCurrentDateTime();

      // Add to the database.
      __int64 iUIDID = PersistentFetchAccountUID::AddUID(faid_, sUIDValue);

      shared_ptr<FetchAccountUID> pUID = shared_ptr<FetchAccountUID>(new FetchAccountUID(iUIDID, faid_, sUIDValue, sCreateTime));

      fetched_uids_.insert(std::make_pair(sUIDValue, pUID));
   }

   bool 
   FetchAccountUIDList::IsUIDInList(const String&sUID) const
   {
      std::map<String, shared_ptr<FetchAccountUID> >::const_iterator iter = fetched_uids_.find(sUID);
      if (iter == fetched_uids_.end())
         return false;
      
      return true;
   }

   void 
   FetchAccountUIDList::DeleteUID(const String &sUID)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iter = fetched_uids_.find(sUID);
      if (iter == fetched_uids_.end())
         return;

      shared_ptr<FetchAccountUID> pUID = (*iter).second;

      // Delete from the database
      PersistentFetchAccountUID::DeleteUID(pUID->GetID());

      // Delete from vector in memory.
      fetched_uids_.erase(iter);
   }

   void 
   FetchAccountUIDList::DeleteUIDsNotInSet(set<String> &setUIDs)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iterFA = fetched_uids_.begin();
      std::map<String, shared_ptr<FetchAccountUID> >::iterator iterEnd = fetched_uids_.end();
      while (iterFA != iterEnd)
      {
         shared_ptr<FetchAccountUID> pUID = (*iterFA).second;

         if (setUIDs.find(pUID->GetUID()) == setUIDs.end())
         {
            // No, the UID stored in the database is not 
            // in the vector over valid UID's.

            // Delete from the database
            PersistentFetchAccountUID::DeleteUID(pUID->GetID());

            // Delete from vector in memory.
            iterFA = fetched_uids_.erase(iterFA);
         }
         else
            iterFA++;
      }
   }


   shared_ptr<FetchAccountUID> 
   FetchAccountUIDList::GetUID(const String &sUID)
   {
      std::map<String, shared_ptr<FetchAccountUID> >::const_iterator iter = fetched_uids_.find(sUID);
      if (iter == fetched_uids_.end())
      {

         shared_ptr<FetchAccountUID> pEmpty;
         return pEmpty;
      }

      return (*iter).second;
   }

}