// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "PrerequisiteList.h"

#include "PreReqNoDuplicateFolders.h"
#include "IPrerequisite.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PrerequisiteList::PrerequisiteList(void)
   {
      Initialize_();
   }

   PrerequisiteList::~PrerequisiteList(void)
   {
   }

   void 
   PrerequisiteList::Initialize_()
   {
      shared_ptr<PreReqNoDuplicateFolders> duplicateFolderCheck = shared_ptr<PreReqNoDuplicateFolders>(new PreReqNoDuplicateFolders);
      prerequisites_.push_back(std::make_pair(duplicateFolderCheck->GetDatabaseVersion(), duplicateFolderCheck));
   }

   bool
   PrerequisiteList::Ensure(shared_ptr<DALConnection> connection, int scriptDatabaseVersion, String &sErrorMessage)
   {
      vector<pair<int, shared_ptr<IPrerequisite > > >::iterator iter = prerequisites_.begin();
      vector<pair<int, shared_ptr<IPrerequisite > > >::iterator iterEnd = prerequisites_.end();

      for (; iter != iterEnd; iter++)
      {
         int databaseVersion = (*iter).first;

         if (databaseVersion == scriptDatabaseVersion)
         {
            shared_ptr<IPrerequisite > prereq = (*iter).second;

            if (!prereq->Ensure(connection, sErrorMessage))
               return false;
         }
      }

      return true;
   }

   
}