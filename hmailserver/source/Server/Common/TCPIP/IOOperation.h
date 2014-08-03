// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2005-07-21

#pragma once

namespace HM
{
   class ByteBuffer;

   class IOOperation
   {
   public:

      enum OperationType
      {
         BCTSend,
         BCTReceive,
         BCTShutdownSend,
         BCTDisconnect,
      };

      IOOperation(OperationType type, shared_ptr<ByteBuffer> buffer);
      IOOperation(OperationType type, const AnsiString &string);
      ~IOOperation(void);

      OperationType GetType() {return type_; }
      shared_ptr<ByteBuffer> GetBuffer() {return buffer_; }
      AnsiString GetString() {return string_; }

   private:

      OperationType type_;
      AnsiString string_;
      shared_ptr<ByteBuffer> buffer_;

   };
}