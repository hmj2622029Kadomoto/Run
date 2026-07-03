#include "DxLib.h"
#include "Run.h"
#include <string.h>

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_BADFOOD_MAX = 9; // ダメな食べ物の画像の枚数（種類）
const int BADFOOD_MAX = 1000; // ダメな食べ物の数の最大値
const int IMG_GOODFOOD_TYPE = 7; // 良い食べ物の種類
const int GOODFOOD_MAX = 1000; // 良い食べ物の最大数
const int ATTACK_MAX = 1000;
const int STAGE_DISTANCE = FPS * 60; // ステージの長さ
enum { ONION, CHOCOLATE, GRAPE, COFFEE, BEER, AVOCADO, RAW_FISH, MILK, DOG_FOOD,	BADFOOD_KINDNUM }; // ダメな食べ物の種類
enum { COOKED_FISH, VEGETABLES, APPLE, WATERMELON, BANANA, CAT_FOOD, CAT_MILK,	GOODFOOD_KINDNUM }; // 良い食べ物の種類
enum { TITLE, PLAY, OVER, CLEAR }; // シーンを分けるための列挙定数
const char BadFoodImageName[BADFOOD_KINDNUM][10] = { "Onion","Chocolate","Grape","Coffee","Beer","Avocado","RawFish","Milk","DogFood" };
const char GoodFoodImageName[GOODFOOD_KINDNUM][13] = { "WaterMelon","Apple","Banana","Carrot","CatFood","CatMilk","CookedFish" };
// グローバル変数
struct OBJECT player; // 自機用の構造体変数 
struct OBJECT attack[ATTACK_MAX]; // 攻撃用の構造体変数


// ここでゲームに用いる変数や配列を定義する
int imgGround;
int imgSky;
int imgMountain;
int imgCat; // 自機の画像
int imgClaw;
int imgBAD[BADFOOD_KINDNUM]; // 敵機の画像
int imgGOOD[GOODFOOD_KINDNUM]; // アイテムの画像
//int bgm, jinOver, jinClear, seExpl, seItem, seShot; // 音の読み込み用
int distance = 0; // ステージ終端までの距離
//int bossIdx = 0;
//int stage = 1; // ステージ
int score = 0; // スコア
int hisco = 10000; // ハイスコア
int noDamageFrame = 0; // 無敵状態
int scene = TITLE; // シーンを管理
int timer = 0; // 時間の進行を管理

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("走る"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(TRUE); // ウィンドウモードで起動
	if (DxLib_Init() == -1) { return  -1; } // ライブラリ初期化　エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	InitGame();
	InitVariable();

	while (1) // メインループ
	{
		ClearDrawScreen(); // 画面をクリアする

		// ゲームの骨組みプログラムをここに記入する

		ScrollBG(1);

		MovePlayer();
		MoveAttack();
		ScreenFlip(); // 裏画面を表画面に反映させる
		WaitTimer(1000 / FPS); // 一定時間待つ
		if (ProcessMessage() == -1) { break; } // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) { break; }
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0;
}

void InitGame(void) // Initは初期化という意味（initialize）
{
	// 背景用の画像の読み込み
	imgSky = LoadGraphWithCheck("repo用素材フォルダー/Sky.png");
	imgMountain = LoadGraphWithCheck("repo用素材フォルダー/Mountain.png");
	imgGround = LoadGraphWithCheck("repo用素材フォルダー/Ground.png");

	// 自機と自機の弾の画像の読み込み
	imgCat = LoadGraphWithCheck("repo用素材フォルダー/Cat.png");
	imgClaw = LoadGraphWithCheck("repo用素材フォルダー/Claw.png");

	for (int i = 0; i < BADFOOD_KINDNUM; i++)
	{
		char fileName[50] = "repo用素材フォルダー/";
		strcat_s(fileName, BadFoodImageName[i]);
		strcat_s(fileName, ".png");
		imgBAD[i] = LoadGraphWithCheck(fileName);
	}

	for (int i = 0; i < GOODFOOD_KINDNUM; i++)
	{
		char fileName[50] = "repo用素材フォルダー/";
		strcat_s(fileName, GoodFoodImageName[i]);
		strcat_s(fileName, ".png");
		imgGOOD[i] = LoadGraphWithCheck(fileName);
	}
	//imgBullet = LoadGraphWithCheck("image/bullet.png");
	// 敵の画像の読み込み
	// その他の画像の読み込み

	//// サウンドの読み込みと音量設定
	//bgm = LoadSoundMemWithCheck("sound/bgm.mp3");
	//jinOver = LoadSoundMemWithCheck("sound/gameover.mp3");
	//jinClear = LoadSoundMemWithCheck("sound/stageclear.mp3");
	//seExpl = LoadSoundMemWithCheck("sound/explosion.mp3");
	//seItem = LoadSoundMemWithCheck("sound/item.mp3");
	//seShot = LoadSoundMemWithCheck("sound/shot.mp3");
	//ChangeVolumeSoundMem(128, bgm);
	//ChangeVolumeSoundMem(128, jinOver);
	//ChangeVolumeSoundMem(128, jinClear);
}

void ScrollBG(int spd) 
{
	static int GroundX,MountainX,SkyX;
	SkyX = (SkyX - spd) % WIDTH;
	DrawGraph(SkyX + WIDTH, 0, imgSky,TRUE);
	DrawGraph(SkyX, 0, imgSky,TRUE);
	MountainX = (MountainX - spd * 2) % WIDTH;
	DrawGraph(MountainX + WIDTH, 0, imgMountain, TRUE);
	DrawGraph(MountainX, 0, imgMountain,TRUE);
	DrawGraph(GroundX, 0, imgGround, TRUE);
}

void InitVariable(void)
{
	player.x = WIDTH / 4;
	player.y = HEIGHT * 3 / 4;
	player.vx = 5;
	player.vy = 5;
}

void DrawImage(int img, int x, int y)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	DrawGraph(x - w / 2, y - h / 2, img, TRUE);
}

void MovePlayer(void)
{
	int ix;
	static int count = 0;
	ix = player.timer * 94; // 画像の切り出し位置

	if (CheckHitKey(KEY_INPUT_W)) {
		player.y -= player.vy;
		if (player.y < HEIGHT / 2 ) { player.y = HEIGHT / 2; }
	}
	if (CheckHitKey(KEY_INPUT_S)) {
		player.y += player.vy;
		if (player.y > HEIGHT - 23) { player.y = HEIGHT - 23; }
	}
	if (CheckHitKey(KEY_INPUT_A)) {
		player.x -= player.vx;
		if (player.x < 47) { player.x = 47; }
	}
	if (CheckHitKey(KEY_INPUT_D)) {
		player.x += player.vx;
		if (player.x > WIDTH - 47) { player.x = WIDTH - 47; }
	}
	if (GetMouseInput()&MOUSE_INPUT_LEFT) {

	}
	DrawRectGraph(player.x - 47, player.y - 46, ix, 0, 94, 92, imgCat, TRUE, FALSE);
	count++;
	if (count % 4 == 0) {
		player.timer++;
		if (player.timer  >= 7) { player.timer = 0; }
	}
}





// 画像の読み込み、読み込み失敗時は通知
int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "画像の読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}