#include "stdafx.h"
#include "StreamServer.h"

#include <sys/types.h>
#include <sys/stat.h>

void StreamServer::StopStream()
{
    m_continue = false;
    if (m_partial)
    {
        WaitForSingleObject(m_partial->m_hThread, INFINITE);
        m_partial = NULL;
    }
    if (m_entire)
    {
        WaitForSingleObject(m_entire->m_hThread, INFINITE);
        m_entire = NULL;
    }
}


StreamServer::StreamServer(LPSTR filepath, Controller *controller)
{
    m_file = NULL;
    m_filepath = NULL;
    m_bottomBoundary = 0;
    m_upperBoundary = 0;
    this->m_filepath = filepath;
    this->controller = controller;
    m_continue = true;
}

StreamServer::~StreamServer()
{
}

void StreamServer::StartPartialStreamThread()
{
    m_partial = AfxBeginThread(StreamServer::PartialStreamThreadFunction, this);
}

void StreamServer::OnReceive(int nErrorCode)
{
   if (nErrorCode)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }
    CHAR buffer[500];
    int recv = Receive(buffer, sizeof(buffer));
    if (recv == SOCKET_ERROR)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }
	else
	{
        if (recv>0)
        {
            buffer[recv] = 0;
            if (strstr(buffer, "GET") && (strstr(buffer, "Range:") || strstr(buffer, "Accept-Encoding:")))
            {
                m_bottomBoundary = 0;
                m_upperBoundary = 0;
                if (LPSTR v = strstr(buffer, "Range: bytes="))
                {
                    v += strlen("Range: bytes=");
                    CHAR value[20];
                    int idx = 0;
                    while (*v && *v != '-')  value[idx++] = *v++;
                    value[idx] = 0;
                    m_bottomBoundary = atoi(value);

                    v++;
                    idx = 0;
                    while (*v && *v != '\r')  value[idx++] = *v++;
                    value[idx] = 0;
                    m_upperBoundary = atoi(value);
                }
                StartPartialStreamThread();
            }
            else if (strstr(buffer, "HEAD"))
                SendHeadMsg();
            else
            {
                BYTE *m_sample;
                CHAR TypeExtension[20];
                GetFileTypeAndExtension(TypeExtension);
                CHAR CompleteMSG[1000];
                if (fopen_s(&m_file, m_filepath, "rb"))
                {
                    if (controller)
                        controller->OnFileOpenError();
                    return;
                }
                CHAR FULLMSG[] = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n";
                if (GetFileSize() < 104857600) //100 MB
                {
                    m_sample = new BYTE[GetFileSize()];
                    fread(m_sample, GetFileSize(), 1, m_file);
                    sprintf_s(CompleteMSG, sizeof(CompleteMSG), FULLMSG, GetFileSize(), TypeExtension);
                    Send(CompleteMSG, strlen(CompleteMSG));
                    Send(m_sample, GetFileSize());
                    fclose(m_file);
                    delete[] m_sample;
                    m_sample = NULL;
                } 
                else
                    StartEntireContentStreamThread();
            }
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}

void StreamServer::PartialStream()
{
    BYTE *m_sample;
    CHAR TypeExtension[20];
    GetFileTypeAndExtension(TypeExtension);
    const CHAR FailReqRange[] = "HTTP/1.1 416 Requested Range Not Satisfiable\r\nContent-Range: bytes */%d\r\nTransferMode.DLNA.ORG: Streaming\r\nConnection: close\r\nContent-Length: 0";
    const CHAR MSG[] = "HTTP/1.1 206 Partial Content\r\nContent-Length: %d\r\nContent-Type: %s\r\nContent-Range: bytes %d-%d/%d\r\nTransferMode.DLNA.ORG: Streaming\r\n\r\n";

    CHAR CompleteMSG[1000];
    UINT fileSize = GetFileSize();
    if (fopen_s(&m_file, m_filepath, "rb"))
    {
        if (controller)
            controller->OnFileOpenError();
        return;
    }
    UINT part = 104857600; //100 MB

    if (m_bottomBoundary > 0 && m_bottomBoundary <= fileSize && m_upperBoundary > 0 && m_upperBoundary <= fileSize)
    {
        fseek(m_file, m_bottomBoundary, SEEK_SET);
        m_sample = new BYTE[m_upperBoundary - m_bottomBoundary + 1];
        fread(m_sample, m_upperBoundary - m_bottomBoundary + 1, 1, m_file);
        sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, m_upperBoundary - m_bottomBoundary + 1, TypeExtension, m_bottomBoundary, m_upperBoundary, fileSize);   
        Send(CompleteMSG, strlen(CompleteMSG));
        Send(m_sample, m_upperBoundary - m_bottomBoundary + 1);
    }
    else if (m_bottomBoundary == 0 && m_upperBoundary == 0)
    {
        if (fileSize > part)
        {
            UINT fullLoops = fileSize / part;
            UINT extraLoop = fileSize % part;
            UINT i = 0;
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, GetFileSize(), TypeExtension, m_bottomBoundary, GetFileSize() - 1, fileSize);
            Send(CompleteMSG, strlen(CompleteMSG));
            m_sample = new BYTE[part];
            while (i < fullLoops && m_continue)
            {           
                fread(m_sample, part, 1, m_file);
                Send(m_sample, part);
                //Sleep(1000);
                i++;
            }
            delete[] m_sample;
            m_sample = NULL;
            if (extraLoop && m_continue)
            {
                m_sample = new BYTE[extraLoop];
                fread(m_sample, extraLoop, 1, m_file);
                Send(m_sample, extraLoop);
            }
        }
        else
        {
            CHAR FULLMSG[] = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\nTransferMode.DLNA.ORG: Streaming\r\n\r\n";
            m_sample = new BYTE[fileSize];
            fread(m_sample, fileSize, 1, m_file);
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), FULLMSG, fileSize, TypeExtension);
            Send(CompleteMSG, strlen(CompleteMSG));
            Send(m_sample, fileSize);
        }
    }
    else if (m_bottomBoundary == 0 && m_upperBoundary > 0 && m_upperBoundary <= fileSize)
    {
        m_sample = new BYTE[m_upperBoundary + 1];
        fread(m_sample, m_upperBoundary + 1, 1, m_file);
        sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, m_upperBoundary - m_bottomBoundary + 1, TypeExtension, m_bottomBoundary, m_upperBoundary, fileSize);
        Send(CompleteMSG, strlen(CompleteMSG));
        Send(m_sample, m_upperBoundary);
    }
    else if (m_bottomBoundary > 0 && m_bottomBoundary <= fileSize && m_upperBoundary == 0)
    {
        fseek(m_file, m_bottomBoundary, SEEK_SET);
        UINT rest = fileSize - m_bottomBoundary;
        sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, fileSize - m_bottomBoundary, TypeExtension, m_bottomBoundary, fileSize - 1, fileSize);
        Send(CompleteMSG, strlen(CompleteMSG));
        if (rest > part)
        {
            UINT fullLoops = fileSize / part;
            UINT extraLoop = fileSize % part;
            UINT i = 0;
            m_sample = new BYTE[part];
            while (i < fullLoops && m_continue)
            {
                fread(m_sample, part, 1, m_file);
                Send(m_sample, part);
                //Sleep(1000);
                i++;
            }
            delete[] m_sample;
            m_sample = NULL;
            if (extraLoop && m_continue)
            {
                m_sample = new BYTE[extraLoop];
                fread(m_sample, extraLoop, 1, m_file);
                Send(m_sample, extraLoop);
            }
        }
        else
        {
            m_sample = new BYTE[fileSize - m_bottomBoundary+1];
            fread(m_sample, fileSize - m_bottomBoundary, 1, m_file);
            Send(m_sample, fileSize - m_bottomBoundary + 1);
        }
    }
    else
    {
        m_sample = new BYTE[1];
        sprintf_s(CompleteMSG, sizeof(CompleteMSG), FailReqRange, fileSize);
        Send(CompleteMSG, strlen(CompleteMSG));
    }
    
    fclose(m_file);
    if (m_sample)
    {
        delete[] m_sample;
        m_sample = NULL;
    }
}

UINT StreamServer::PartialStreamThreadFunction(LPVOID param)
{
    StreamServer* This = (StreamServer*)param;
    This->PartialStream();
    return 0;
}


BOOL StreamServer::SendHeadMsg()
{   
    CHAR TypeExtension[20];
    const CHAR MSG[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: %s\r\nTransferMode.DLNA.ORG: Streaming\r\nConnection: close\r\n\r\n";
    CHAR CompleteMSG[500];
    
    sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, GetFileSize(), GetFileTypeAndExtension(TypeExtension));

    if (!Send(CompleteMSG, strlen(CompleteMSG)))
        return FALSE;
    return TRUE;

}

StreamServer::StreamServer(const StreamServer &other)
{
    m_bottomBoundary = other.m_bottomBoundary;
    m_upperBoundary = other.m_upperBoundary;
    m_file = other.m_file;
    m_filepath = other.m_filepath;
    m_continue = bool(other.m_continue);
}

StreamServer &StreamServer::operator=(const StreamServer& other)
{
    m_file = other.m_file;
    m_bottomBoundary = other.m_bottomBoundary;
    m_upperBoundary = other.m_upperBoundary;
    m_filepath = other.m_filepath;
    m_continue = bool(other.m_continue);
    return *this;
}

UINT StreamServer::GetFileSize()
{
    struct stat st;
    stat(m_filepath, &st);
    return st.st_size;
}

LPSTR StreamServer::GetFileTypeAndExtension(LPSTR TypeExtension)
{
    CHAR image[] = "image";
    CHAR video[] = "video";
    CHAR audio[] = "audio";
    CHAR jpeg[] = "jpeg";

    LPSTR v = strrchr(m_filepath, '.');
    if (!v) return "";
    //FIRE format not supported  

    //
    v += 1;

    CHAR ext[5];

    int i = 0;
    while (*v && *v != '\0')  ext[i++] = tolower(*v++);
    ext[i] = '\0';

    if (!strcmp(ext, "avi") || !strcmp(ext, "mp4") || !strcmp(ext, "mkv"))
        strcpy_s(TypeExtension, 20, video);
    else if (!strcmp(ext, "wav") || !strcmp(ext, "wave") || !strcmp(ext, "mp3"))
        strcpy_s(TypeExtension, 20, audio);
    else if (!strcmp(ext, "jpg") || !strcmp(ext, "bmp") || !strcmp(ext, "png") || !strcmp(ext, "gif") || !strcmp(ext, "tif") || !strcmp(ext, "tiff"))
        strcpy_s(TypeExtension, 20, image);

    strcat_s(TypeExtension, 20, "/");

    if (!strcmp(ext, "jpg"))
        strcat_s(TypeExtension, 20, "jpeg");
    if (!strcmp(ext, "tif"))
        strcat_s(TypeExtension, 20, "tiff");
    else if (!strcmp(ext, "mp3"))
        strcat_s(TypeExtension, 20, "mpeg");
    else if (!strcmp(ext, "mkv"))
        strcat_s(TypeExtension, 20, "x-matroska");
    else
        strcat_s(TypeExtension, 20, ext);

    TypeExtension[strlen(TypeExtension)] = '\0';
    
    return TypeExtension;
}

void StreamServer::StartEntireContentStreamThread()
{
    m_entire = AfxBeginThread(StreamServer::EntireStreamThreadFuntion, this);
}

UINT StreamServer::EntireStreamThreadFuntion(LPVOID param)
{
    StreamServer* This = (StreamServer*)param;
    This->EntireStream();
    return 0;
}

void StreamServer::EntireStream()
{
    BYTE *m_sample;
    CHAR TypeExtension[20];
    GetFileTypeAndExtension(TypeExtension);
    CHAR CompleteMSG[1000];
    if (fopen_s(&m_file, m_filepath, "rb"))
    {
        if (controller)
            controller->OnFileOpenError();
        return;
    }
    CHAR FULLMSG[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nContent-Type: %s\r\nTransferMode.DLNA.ORG: Streaming\r\nConnection: close\r\n\r\n\0";
    UINT compl_parts = GetFileSize() / 104857600;
    UINT final_part = GetFileSize() % 104857600;
    UINT i = 0;
    sprintf_s(CompleteMSG, sizeof(CompleteMSG), FULLMSG, GetFileSize(), TypeExtension);
    Send(CompleteMSG, strlen(CompleteMSG));
    m_sample = new BYTE[104857600];
    while (i < compl_parts && m_continue)
    {
        fread(m_sample, 104857600, 1, m_file);
        Send(m_sample, 104857600);
        //Sleep(1000);
        i++;
    }
    delete[] m_sample;
    m_sample = NULL;
    if (final_part && m_continue)
    {
        m_sample = new BYTE[final_part];
        fread(m_sample, final_part, 1, m_file);
        Send(m_sample, final_part);
        delete[] m_sample;
        m_sample = NULL;
    }
    fclose(m_file);
}