#include "DxLib.h"
#include "Run.h"

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_ENEMY_MAX = 5; // 敵の画像の枚数（種類）
const int BULLET_MAX = 100;
const int ENEMY_MAX = 100; // 敵機の数の最大値
const int STAGE_DISTANCE = FPS * 60; // ステージの長さ
const int ITEM_TYPE = 3; // アイテムの種類
const int ITEM_MAX = 1000; // アイテムの最大数
const int WEAPON_LV_MAX = 10; // 武器レベルの最大値
const int PLAYER_SPEED_MAX = 20; // 自機の速さの最大値
enum { ONION, CHOCOLATE, GRAPE, COFFEE, BEER, AVOCADO, RAW_FISH, MILK, DOG_FOOD }; // 敵機の種類
enum { , ENE_ZAKO1, ENE_ZAKO2, ENE_ZAKO3, ENE_BOSS }; // 敵機の種類
enum { TITLE, PLAY, OVER, CLEAR }; // シーンを分けるための列挙定数

// グローバル変数
struct OBJECT player; // 自機用の構造体変数 

// ここでゲームに用いる変数や配列を定義する
int imgStage;
int imgCat; // 自機の画像
//int imgEnemy[IMG_ENEMY_MAX]; // 敵機の画像
//int imgExprosion; // 爆発演出の画像
//int imgItem; // アイテムの画像
//int bgm, jinOver, jinClear, seExpl, seItem, seShot; // 音の読み込み用
//int distance = 0; // ステージ終端までの距離
//int bossIdx = 0;
//int stage = 1; // ステージ
//int score = 0; // スコア
//int hisco = 10000; // ハイスコア
//int noDamageFrame = 0; // 無敵状態
//int weaponLv = 1; // 自機の武器のレベル（同時に発射される弾数）
//int scene = TITLE; // シーンを管理
//int timer = 0; // 時間の進行を管理

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

		ScrollBG(5);

		MovePlayer();
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
	imgStage = LoadGraphWithCheck("repo用素材フォルダー/Stage.png");
	//imgFloor = LoadGraphWithCheck("image/bg1.png");
	//imgWallL = LoadGraphWithCheck("image/bg2.png");
	//imgWallR = LoadGraphWithCheck("image/bg3.png");
	// 自機と自機の弾の画像の読み込み
	imgCat = LoadGraphWithCheck("repo用素材フォルダー/Cat.png");
	//imgBullet = LoadGraphWithCheck("image/bullet.png");
	//// 敵の画像の読み込み
	//for (int i = 0; i < IMG_ENEMY_MAX; i++) {
	//	char file[] = "image/enemy*.png";
	//	file[11] = (char)('0' + i);
	//	imgEnemy[i] = LoadGraphWithCheck(file);
	//}
	//// その他の画像の読み込み
	//imgExprosion = LoadGraphWithCheck("image/explosion.png"); // 爆発演出
	//imgItem = LoadGraphWithCheck("image/item.png"); // アイテム

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
	static int stageX;
	stageX = (stageX - spd) % WIDTH;
	DrawGraph(stageX + WIDTH, 0, imgStage, FALSE);
	DrawGraph(stageX, 0, imgStage, FALSE);
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

	if (CheckHitKey(KEY_INPUT_UP)) {
		player.y -= player.vy;
		if (player.y < HEIGHT / 2 ) { player.y = HEIGHT / 2; }
	}
	if (CheckHitKey(KEY_INPUT_DOWN)) {
		player.y += player.vy;
		if (player.y > HEIGHT - 23) { player.y = HEIGHT - 23; }
	}
	if (CheckHitKey(KEY_INPUT_LEFT)) {
		player.x -= player.vx;
		if (player.x < 47) { player.x = 47; }
	}
	if (CheckHitKey(KEY_INPUT_RIGHT)) {
		player.x += player.vx;
		if (player.x > WIDTH-47) { player.x = WIDTH - 47; }
	}
	DrawRectGraph(player.x-47, player.y-46,ix,0,94,92,imgCat,TRUE,FALSE);
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