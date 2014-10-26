#include "DxLib.h"

#define Screen_X 720
#define Screen_Y 510

int Center(int GraphSize, char Type);
void Struct(int PlayType, float Rate, int Frame, int Elapsed, float BPM);
int UpdateKey(char Key []);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//ログがなんたら
	SetOutApplicationLogValidFlag(FALSE);
	// タイトルを変更
	SetMainWindowText("Let's Dancing YBK!!");
	// ウインドウモードに変更
	ChangeWindowMode( TRUE ) ;
	//ウインドウの大きさ設定
	SetGraphMode(Screen_X, Screen_Y, 32);
	//ウインドウの大きさを自由に変更出来るかどうかのフラグ
	SetWindowSizeChangeEnableFlag( TRUE ) ;
	// 裏画面を使用
	SetDrawScreen(DX_SCREEN_BACK);
	// ＤＸライブラリの初期化
	if (DxLib_Init() == -1) return -1;

	int Gray;
	int i, j, k;
	int Count = 0, PlayType = 0 ;
	float fCount = 0.0, Rate, PlayRate, ReverseRate;
	int YBKSNPI[60], Size_X, Size_Y, Point_X, Point_Y;
	int mem[60];
	char StrBuf[128], StrBuf2[32], AddressText[60];
	char Key[256];
	float Frame, BPM;
	int ElapsedTime, sum=0;
	int loop=0;

	Gray = GetColor(115, 124, 107);

	//画像読み込み(57枚分)
	j = 56;
	for(i = 0; i < 57; i++){
		sprintf(AddressText , "YBKSNPI/ybk%d.jpg" , i) ;
		puts( AddressText ) ;
		YBKSNPI[j] = LoadGraph( AddressText ) ;
//		YBKSNPI[i] = LoadGraph( AddressText ) ;
		j--;
	}

	//56枚に間引く
	for(i=0;i<56;i++){
		YBKSNPI[i] = YBKSNPI[i+1];
	}

	GetGraphSize(YBKSNPI[0], &Size_X, &Size_Y);

	Point_X = Center(Size_X, 'X');
	Point_Y = 30;

	BPM = 120;

	ElapsedTime = GetNowCount();

	//メインループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		ClearDrawScreen();
		UpdateKey(Key);

		DrawBox(0, 0, Screen_X , Screen_Y, Gray, TRUE);

		//YBKSNPIを描画
		DrawGraph(Point_X, Point_Y, YBKSNPI[Count], TRUE);

		//文字表示系
		if(loop==0)
			Struct(PlayType, Rate, Count, 0, BPM);
		else		Struct(PlayType, Rate, Count, sum / loop, BPM);
 		
 		//いい感じの時間になるように初期レート設定
		PlayRate	= 1.2982346 * BPM / 120;
		ReverseRate = 1.2982346 * BPM / 120;
		//Core i5 : 1.3169519
		//Core i7 : 1.2982346

		Rate = PlayRate;

		//↑ or → でBPMプラス
		if(Key[KEY_INPUT_UP] != 0 || Key[KEY_INPUT_RIGHT] == 1){
			BPM++;
		}
		//↓ or ← でBPMマイナス
		if(Key[KEY_INPUT_DOWN] != 0 || Key[KEY_INPUT_LEFT] == 1){
			BPM--;
			if(BPM < 1)	BPM = 1;
		}

		//プレイタイプによってプラス or マイナス
		if(PlayType == 0)	fCount += Rate;
		else	fCount -= Rate;
		Count = (int)fCount;
		
		//カウントが上限下限になったらカウントを丸める、プレイタイプ変更
		if(Count < 0 || 55 < Count){
			if(Count > 55){
				Count = 55;
				fCount = 55.0;
				PlayType = 1;
				Rate = ReverseRate;
			}
			if(Count < 0){
				Count = 0;
				fCount = 0.0;
				PlayType = 0;
				Rate = PlayRate;
			}

			//時間計算
			ElapsedTime = GetNowCount() - ElapsedTime;
			sum += ElapsedTime;
			loop++;
			ElapsedTime = GetNowCount();
		}
		
		ScreenFlip();
		if (ProcessMessage() == -1)	break;
	}

	DxLib_End();
	return 0;
}

int Center(int GraphSize, char Type){
	if (Type == 'X')	return((Screen_X - GraphSize) / 2);
	if (Type == 'Y')	return((Screen_Y - GraphSize) / 2);
	else return 0;
}

//キーの押下状態
int UpdateKey(char Key[]){
	char tmpKey[256];
	GetHitKeyStateAll(tmpKey);
	for (int i = 0; i<256; i++){
		if (tmpKey[i] != 0)		Key[i]++;
		else	Key[i] = 0;
	}
	return 0;
}

//色々表示
void Struct(int PlayType, float Rate, int Frame, int Elapsed, float BPM){
	char StrBuf[128], StrBuf2[32];
	int Black = GetColor(18, 18, 18);
	int Red = GetColor(238, 53, 42);
	int Rint = (int)(Rate * 10) / 10 ;
	int Rpoint = (int)(Rate * 100000000) % 100000000;
	int Bint = (int)(BPM * 10) / 10 ;

	lstrcpy(StrBuf, "");
	if(PlayType){
		lstrcat(StrBuf, "Reverse");
	}
	else{
		lstrcat(StrBuf, "Play   ");
	}
	lstrcat(StrBuf, " Rate :");
	itoa(Rint, StrBuf2, 10);
	lstrcat(StrBuf, StrBuf2);
	lstrcat(StrBuf, ".");
	itoa(Rpoint, StrBuf2, 10);
	lstrcat(StrBuf, StrBuf2);
	lstrcat(StrBuf, " Frame :");
	itoa(Frame, StrBuf2, 10);
	lstrcat(StrBuf, StrBuf2);

	DrawString(0, 0, StrBuf, Black);

	lstrcpy(StrBuf, "Elapsed time ave :");
	itoa(Elapsed, StrBuf2, 10);
	lstrcat(StrBuf, StrBuf2);
	lstrcat(StrBuf, "[ms]");
	lstrcat(StrBuf, " BPM :");
	itoa(Bint, StrBuf2, 10);
	lstrcat(StrBuf, StrBuf2);

	DrawString(0, 15, StrBuf, Black);

	lstrcpy(StrBuf, "＿人人人人人人人人人＿");
	DrawString(Center(0, 'X') - 70, 50, StrBuf, Red);
	lstrcpy(StrBuf, "＞Let's Dancing!!!!＜");
	DrawString(Center(0, 'X') - 70, 65, StrBuf, Red);
	lstrcpy(StrBuf, "^Y^Y^Y^Y^Y^Y^Y^Y^Y^");
	DrawString(Center(0, 'X') - 60, 80, StrBuf, Red);

}
