//=============================================================================
//
// �e���� [sound.cpp]
// Author : SOMEYA KOSUKE
////=============================================================================

#include "main.h"
#include "se.h"

// �_�C���N�g�T�E���h�̃��C�u����
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

// �T�E���h�t�@�C���̃p�X�ih�̒ʂ��i���o�[�ƍ��킹�邱�Ɓj
const TCHAR* soundFilename[] = {
	_T("data/SE/hit.wav"),
	_T("data/SE/dorum .wav"),
	_T("data/SE/shimbal.wav"),
	
};

// �O���[�o���ϐ�
static IDirectSound8 *DirectSound = NULL;	// �T�E���h�C���^�[�t�F�[�X
static LPDIRECTSOUNDBUFFER8         SE[3]; //SE�p�̃T�E���h�o�b�t�@�[


// �T�E���h�̏�����
// hWnd:�E�B���h�E�n���h��
HRESULT InitSE( HWND hWnd )
{
	// DirectSound�I�u�W�F�N�g�̍쐬
	if (FAILED(DirectSoundCreate8(NULL, &DirectSound, NULL)))
		return E_FAIL;
	
	// �������x���ݒ� 
	if (FAILED(DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return E_FAIL;

	for (int i = 0; i < SE_MAX; i++)
	{
		SE[i] = LoadSE(i);
	}


	return S_OK;
}

// ��Еt��
void UninitSE()
{
	if( DirectSound != NULL )
	{
		DirectSound->Release();
		DirectSound = NULL;
	}
}

// �T�E���h�̃��[�h
// no:�T�E���h�i���o�[�i�w�b�_�ɒ�`���ꂽ�񋓌^�萔�j
LPDIRECTSOUNDBUFFER8 LoadSE( int no )
{
	// MMIO = �}���`���f�B�A���o�́A�̗��B
	LPDIRECTSOUNDBUFFER  BaseBuffer = NULL;		// �ȃf�[�^�̑����o�b�t�@
	LPDIRECTSOUNDBUFFER8 Buffer =  NULL;			// �ȃf�[�^�̃o�b�t�@
	DSBUFFERDESC buff;							// �o�b�t�@�ݒ�\����

	HMMIO hMmio = NULL;							// MMIO�n���h��
	MMIOINFO mmioInfo;							// �}���`���f�B�A�f�[�^�\����

	MMRESULT mmRes;								// MM�������ʊi�[�ϐ�
	MMCKINFO riffChunk, formatChunk, dataChunk;	// �`�����N�f�[�^�\����
	DWORD size;									// �f�[�^�T�C�Y�i�[�p
	WAVEFORMATEX pcm;							// �ȃf�[�^�t�H�[�}�b�g�\����

	LPVOID Block;								// �Z�J���_���o�b�t�@�̃f�[�^�������ݐ�A�h���X
	DWORD  dwSize;								// �Z�J���_���o�b�t�@�̃T�C�Y

	// 1.�n���h�������ƂɃt�@�C�����J��
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)soundFilename[no], &mmioInfo, MMIO_READ);
	if(!hMmio)
		return NULL;

	// 2.�t�@�C����͇@ RIFF�`�����N����
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// �L�[���[�h�����ƂɌ���
	
	if(mmRes != MMSYSERR_NOERROR)		// ������Ȃ�������ُ�I��
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.�t�@�C����͇A �t�H�[�}�b�g�`�����N����
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	
	if(mmRes != MMSYSERR_NOERROR)		// ������Ȃ�������ُ�I��
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// �����������Ƃɓǂݍ���
	
	if(size != formatChunk.cksize) 		// �������ǂݍ��߂Ȃ�������ُ�I��
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0); 

	// 4.�t�@�C����͇B �f�[�^�`�����N����
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	
	if(mmRes != MMSYSERR_NOERROR) 		// ������Ȃ�������ُ�I��
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.�f�[�^�ǂݍ���
	char *Data = new char[dataChunk.cksize];					// �K�v�ȑ傫���̗̈���m�ۂ���
	size = mmioRead(hMmio, (HPSTR)Data, dataChunk.cksize);		// �f�[�^��ǂݍ���
	
	if(size != dataChunk.cksize)  		// �������ǂݍ��߂Ȃ�������ُ�I��
	{
		delete[] Data;
		return NULL;
	}

	// 6.�Ȃ�ǂݍ��ށu�Z�J���_���o�b�t�@�v��p��
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// �܂�������
	buff.dwSize = sizeof(DSBUFFERDESC);				// ��������e��ݒ�
	buff.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;

	// �����o�b�t�@�����
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &BaseBuffer, NULL)))
		return NULL;

	// �T�E���h�o�b�t�@�����o��
	if (FAILED(BaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&Buffer)))
		return NULL;

	// ���o�����瑍���o�b�t�@���̂Ă�
	BaseBuffer->Release();

	// 7.���ꂽ�Z�J���_���o�b�t�@�Ƀf�[�^��]��
	// �܂��͏������݂ł���悤�o�b�t�@�����b�N
	if (FAILED(Buffer->Lock(0, 0, &Block, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
		return NULL;

	memcpy(Block, Data, dwSize);	// �f�[�^�]��
	delete[] Data;					// ���̓ǂݍ��ݗ̈������

	// �o�b�t�@���b�N����
	Buffer->Unlock(Block, dwSize, 0, 0);
	
	// �Z�J���_���o�b�t�@��Ԃ��Ă悤�₭����...
	return Buffer;
}

// ����炷
// pBuffer:����炵�����T�E���h�f�[�^�̃Z�J���_���o�b�t�@
// flag   :1(E_DS8_FLALOOP)�Ȃ烋�[�v�Đ�
void PlaySE(  LPDIRECTSOUNDBUFFER8 Buffer, int flag/*=0*/ )
{	// ��������炷�̂ŁA�ŏ�����炵�����ꍇ��SetCurrentPosition(0)�����邱��
	Buffer->SetCurrentPosition(0);
	Buffer->Play(0, 0, flag);
}

// �����~�߂�
void StopSE( LPDIRECTSOUNDBUFFER8 Buffer )
{
	DWORD status;

	Buffer->GetStatus(&status);
	if( status & DSBSTATUS_PLAYING )	// ���Ă�����
	{
		Buffer->Stop();	// �Ӗ��I�ɂ�Pause�ɂȂ�B
	}
}

// �Đ������ǂ������ׂ�
// pBuffer:����炵�����T�E���h�f�[�^�̃Z�J���_���o�b�t�@
bool IsPlayingSE( LPDIRECTSOUNDBUFFER8 Buffer )
{
	DWORD status;

	Buffer->GetStatus(&status);
	if( status & DSBSTATUS_PLAYING )
	{
		return true;
	}
	return false;
}
//�����Ŗ炵����se���w��
void SetSE(int no)
{

	PlaySE(SE[no], 0);

}