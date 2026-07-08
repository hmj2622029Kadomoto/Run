#include "DxLib.h"
#include "Run.h"
#include <string.h>
#include <stdlib.h>

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_BADFOOD_MAX = 9; // ダメな食べ物の画像の枚数（種類）
const int BADFOOD_MAX = 1000; // ダメな食べ物の数の最大値
const int IMG_GOODFOOD_TYPE = 7; // 良い食べ物の種類
const int GOODFOOD_MAX = 1000; // 良い食べ物の最大数
const int DOG_MAX = 1000;
const int ATTACK_MAX = 1000;
const int STAGE_DISTANCE = FPS * 60; // ステージの長さ
const int PLAYER_HP_MAX = 1000;
enum { ONION, CHOCOLATE, GRAPE, COFFEE, BEER, AVOCADO, RAW_FISH, MILK, DOG_FOOD,	BADFOOD_KINDNUM }; // ダメな食べ物の種類
enum { COOKED_FISH, VEGETABLES, APPLE, WATERMELON, BANANA, CAT_FOOD, CAT_MILK,	GOODFOOD_KINDNUM }; // 良い食べ物の種類
enum { TITLE, PLAY, OVER, CLEAR }; // シーンを分けるための列挙定数
const char BadFoodImageName[BADFOOD_KINDNUM][10] = { "Onion","Chocolate","Grape","Coffee","Beer","Avocado","RawFish","Milk","DogFood" };
const char GoodFoodImageName[GOODFOOD_KINDNUM][13] = { "CookedFish","Carrot","Apple","WaterMelon","Banana","CatFood","CatMilk" };
// グローバル変数
struct OBJECT player; // 自機用の構造体変数 
struct OBJECT attack[ATTACK_MAX]; // 攻撃用の構造体変数
struct OBJECT BadFood[BADFOOD_MAX];
struct OBJECT GoodFood[GOODFOOD_MAX];
struct OBJECT Dog[DOG_MAX];

// ここでゲームに用いる変数や配列を定義する
int imgGround;
int imgSky;
int imgMountain;
int imgCat; // 自機の画像
int imgDog;
int imgClaw;
int imgBAD[BADFOOD_KINDNUM]; // 敵機の画像
int imgGOOD[GOODFOOD_KINDNUM]; // アイテムの画像
//int bgm, jinOver, jinClear, seExpl, seItem, seShot; // 音の読み込み用
int distance = 0; // ステージ終端までの距離
//int bossIdx = 0;
int stage = 1; // ステージ
int score = 0; // スコア
int hisco = 10000; // ハイスコア
int noDamageFrame = 0; // 無敵状態
int scene = TITLE; // シーンを管理
int timer = 0; // 時間の進行を管理
int coffeeCount = 0;
int milkCount = 0;
int darkness = 0;
int reverseTimer = 0;
char effectText[100] = "";
int effectTimer = 0;
int effectColor = GetColor(0, 0, 0);

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
	distance = STAGE_DISTANCE;
	while (1) // メインループ
	{
		ClearDrawScreen(); // 画面をクリアする

		// ゲームの骨組みプログラムをここに記入する

		ScrollBG(player.vx / 2);
		timer++;
		player.hp = player.hp - 1 - milkCount;

		if (reverseTimer > 0) { reverseTimer--; }
		DrawFormatString(0, 0, 0x000000,"distance=%d", distance);

		if (distance > 0) { distance--; }
		if (player.hp > 0) {
			if (distance % 40 == 1)
			{
				int x = WIDTH;
				int y = HEIGHT / 2 + rand() % (HEIGHT - 400);
				int b = GetRand(BADFOOD_KINDNUM - 1);
				if (b == ONION) { SetBadFood(x, y, -player.vx, 0, ONION, imgBAD[ONION], 2); }
				if (b == CHOCOLATE) {
					int vy = 0;
					if (player.y < y - 50) { vy = -3; }
					if (player.y > y + 50) { vy = 3; }
					SetBadFood(x, y, -player.vx * 2, vy, CHOCOLATE, imgBAD[CHOCOLATE], 2);
				}
				if (b == GRAPE) {
					int vy = 0;
					if (timer % 60 < 30)
						vy -= 1;
					else
						vy += 1;
					SetBadFood(x, HEIGHT * 3 / 4, -7, vy, GRAPE, imgBAD[GRAPE], 2);
				}
				if (b == COFFEE) { SetBadFood(100 + rand() % (WIDTH - 200), 0, -player.vx, 10, COFFEE, imgBAD[COFFEE], 2); }
				if (b == BEER) { SetBadFood(100 + rand() % (WIDTH - 200), HEIGHT, -player.vx, -10, BEER, imgBAD[BEER], 2); }
				if (b == AVOCADO) { SetBadFood(x, player.y, -5-GetRand(5), 0, AVOCADO, imgBAD[AVOCADO], 2); }
				if (b == RAW_FISH) { SetBadFood(player.x + 500, 0, -player.vx * 2, 10, RAW_FISH, imgBAD[RAW_FISH], 2); }
				if (b == MILK) { SetBadFood(x, player.y, -1 - timer / 100, 0, MILK, imgBAD[MILK], 2); }
				if (b == DOG_FOOD) { SetBadFood(x, y, -player.vx/2, 0, DOG_FOOD, imgBAD[DOG_FOOD], 2); }

				int g = GetRand(GOODFOOD_KINDNUM - 1);
				if (g == COOKED_FISH) { SetGoodFood(player.x + 500, 0, -player.vx * 2, 10, COOKED_FISH, imgGOOD[COOKED_FISH], 1); } // 生の魚と同じ
				if (g == VEGETABLES) {
					int vy = 0;
					if (player.y < y - 50) { vy = -5; }
					if (player.y > y + 50) { vy = 5; }
					SetGoodFood(x, y, -10, vy, VEGETABLES, imgGOOD[VEGETABLES], 1);  // チョコレートと同じ
				}
				if (g == APPLE) {
					int vy = 0;
					if (timer % 60 < 30)
						vy -= 1;
					else
						vy += 1;
					SetGoodFood(x, HEIGHT * 3 / 4, -3, vy, APPLE, imgGOOD[APPLE], 1);
				} // ぶどうに似ている
				if (g == WATERMELON) { SetGoodFood(x, player.y, -player.vx * 1.5, 0, WATERMELON, imgGOOD[WATERMELON], 1); } // アボカドと同じ
				if (g == BANANA) { SetGoodFood(100 + rand() % (WIDTH - 200), 0, -player.vx, 10, BANANA, imgGOOD[BANANA], 1); } // コーヒーと同じ
				if (g == CAT_FOOD) { SetGoodFood(x, y, -1 - GetRand(20), 0, CAT_FOOD, imgGOOD[CAT_FOOD], 1); } // ドッグフードと同じ
				if (g == CAT_MILK) { SetGoodFood(x, player.y, -1 - timer / 100, 0, CAT_MILK, imgGOOD[CAT_MILK], 1); } // 牛乳と同じ
			}
		}

		MovePlayer();
		MoveAttack();
		MoveBadFood();
		MoveDog();
		MoveGoodFood();
		if (coffeeCount > 0)
		{
			int target = coffeeCount * 50;
			if (target > 255) { target = 255; }
			if (darkness < target) { darkness++; }
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, darkness);
			DrawBox(0, 0, WIDTH, HEIGHT, 0x000000, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		DrawParameter();
		if (effectTimer > 0)
		{
			DrawString(0, 650, effectText, effectColor);
			effectTimer--;
		}
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

	// 敵の画像の読み込み
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
	imgDog = LoadGraphWithCheck("repo用素材フォルダー/Dog.png");
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
	player.vx = 7;
	player.vy = 7;
	player.hp = PLAYER_HP_MAX;
	player.wid = 94;
	player.hei = 92;
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
	static int oldMouse = 0;
	ix = player.timer * 94; // 画像の切り出し位置
	if (player.hp > 0) {
		if (reverseTimer <= 0) {
			if (CheckHitKey(KEY_INPUT_W)) {
				player.y -= player.vy;
				if (player.y < HEIGHT / 2) { player.y = HEIGHT / 2; }
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
		}
		else
		{
			if (CheckHitKey(KEY_INPUT_W)) {
				player.y += player.vy;
				if (player.y > HEIGHT - 23) { player.y = HEIGHT - 23; }
			}
			if (CheckHitKey(KEY_INPUT_S)) {
				player.y -= player.vy;
				if (player.y < HEIGHT / 2) { player.y = HEIGHT / 2; }
			}
			if (CheckHitKey(KEY_INPUT_A)) {
				player.x += player.vx;
				if (player.x > WIDTH - 47) { player.x = WIDTH - 47; }
			}
			if (CheckHitKey(KEY_INPUT_D)) {
				player.x -= player.vx;
				if (player.x < 47) { player.x = 47; }
			}
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) && !(oldMouse & MOUSE_INPUT_LEFT)) {
			SetAttack();
		}
		oldMouse = GetMouseInput();
		if (noDamageFrame > 0) { noDamageFrame--; }
	}
	if (noDamageFrame % 4 < 2) { DrawRectGraph(player.x - 47, player.y - 46, ix, 0, 94, 92, imgCat, TRUE, FALSE); }
	if (player.hp > 0) {
		count++;
		if (count % 4 == 0) {
			player.timer++;
			if (player.timer >= 7) { player.timer = 0; }
		}
	}
	else
	{
		player.vx = 0;
		player.vy = 0;
	}
}

void SetAttack(void)
{
	for (int i = 0; i < ATTACK_MAX; i++) {
		if (attack[i].state == 0) { // 空いている配列にセット
			attack[i].x = player.x + 75;
			attack[i].y = player.y - 10;
			attack[i].state = 1;
			attack[i].timer = 0;
			attack[i].wid = 82;
			attack[i].hei = 30;
			break;
		}
	}
}

void MoveAttack(void){
	int ix;
	static int count = 0;
	for (int i = 0; i < ATTACK_MAX; i++) {
		ix = attack[i].timer * 41; // 画像の切り出し位置
		if (attack[i].state == 0) { continue; } // 空いている配列なら処理しない
		DrawRectExtendGraph(attack[i].x - 41, attack[i].y - 15, attack[i].x + 41, attack[i].y + 15, ix, 0, 41, 15, imgClaw, TRUE);

		count++;
		if (count % 2 == 0) {
			attack[i].timer++;
		}
		if (attack[i].timer >= 5) { attack[i].state = 0; }
	}
}

int SetBadFood(int x, int y, int vx, int vy, int ptn, int img, int hp)
{
	for (int i = 0; i < BADFOOD_MAX; i++) {
		if (BadFood[i].state == 0) {
			BadFood[i].x = x;
			BadFood[i].y = y;
			BadFood[i].vx = vx;
			BadFood[i].vy = vy;
			BadFood[i].state = 1;
			BadFood[i].pattern = ptn;
			BadFood[i].image = img;
			BadFood[i].hp = hp;
			GetGraphSize(img, &BadFood[i].wid, &BadFood[i].hei);
			return i;
		}
	}
	return -1;
}

void MoveBadFood(void)
{
	for (int i = 0; i < BADFOOD_MAX; i++) {
		if (BadFood[i].state == 0) { continue; }
		BadFood[i].x += BadFood[i].vx;
		BadFood[i].y += BadFood[i].vy;
		DrawImage(BadFood[i].image, BadFood[i].x, BadFood[i].y);
		if (BadFood[i].x < -200 || WIDTH + 200 < BadFood[i].x || BadFood[i].y < -200 || HEIGHT + 200 < BadFood[i].y) { BadFood[i].state = 0; }
		if (BadFood[i].hp > 0)
		{
			for (int j = 0; j < ATTACK_MAX; j++) {
				if (attack[j].state == 0) { continue; }
				int dx = abs((int)(BadFood[i].x - attack[j].x));
				int dy = abs((int)(BadFood[i].y - attack[j].y));
				if (dx < BadFood[i].wid / 2 + attack[j].wid / 2 && dy < BadFood[i].hei + attack[j].hei / 2) {
					DamageBadFood(i, 1);
				}
			}
		}
		if (noDamageFrame == 0)
		{
			int dx = abs((int)(BadFood[i].x - player.x));
			int dy = abs((int)(BadFood[i].y - player.y));
			if (dx < BadFood[i].wid / 2 + player.wid / 2 && dy < BadFood[i].hei/2 + player.hei / 2) {
				if (player.hp > 0) {
					if (BadFood[i].pattern == ONION) {
						player.hp -= 10;
						player.vx -= 1;
						if (player.vx < 1)
							player.vx = 1;
						strcpy_s(effectText, "たまねぎを食べてしまった！横への移動が苦しくなった...");
						effectColor = GetColor(255,0,0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == CHOCOLATE) {
						player.hp -= 10;
						player.vy -= 1;
						if (player.vy < 1)
							player.vy = 1;
						strcpy_s(effectText, "チョコレートを食べてしまった！縦への移動が苦しくなった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == GRAPE) {
						player.hp -= 20;
						strcpy_s(effectText, "ぶどうを食べてしまった！体力が他より少し多く減ってしまった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == COFFEE) {
						player.hp -= 10;
						coffeeCount++;
						strcpy_s(effectText, "コーヒーを飲んでしまった！目の前が少し暗くなった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == BEER) {
						player.hp -= 10;
						reverseTimer = FPS * 5;
						strcpy_s(effectText, "アルコールを取ってしまった！めまいがして移動が反転してしまった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == AVOCADO) {
						player.hp -= 10;
						player.vx -= 1;
						if (player.vx < 1)
							player.vx = 1;
						player.vy -= 1;
						if (player.vy < 1)
							player.vy = 1;
						strcpy_s(effectText, "アボカドを食べてしまった！縦横の移動が苦しくなった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == RAW_FISH) {
						player.hp -= 10;
						player.vx -= 2;
						if (player.vx < 1)
							player.vx = 1;
						player.vy -= 2;
						if (player.vy < 1)
							player.vy = 1;
						strcpy_s(effectText, "生魚を食べてしまった！縦横の移動がとても苦しくなった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
					if (BadFood[i].pattern == MILK) {
						player.hp += 150;
						milkCount++;
						player.vx -= 2;
						if (player.vx < 1)
							player.vx = 1;
						player.vy -= 2;
						if (player.vy < 1)
							player.vy = 1;
						strcpy_s(effectText, "人間用の牛乳を飲んでしまった！一時的に回復したが、HPが減りやすくなった...");
						effectColor = GetColor(255, 0, 0);
						effectTimer = FPS * 2;
					}
				}
				if (BadFood[i].pattern == DOG_FOOD) {
					player.hp -= 1;
					player.vx -= 10;
					if (player.vx < 0)
						player.vx = 1;
					player.vy -= 10;
					if (player.vy < 0)
						player.vy = 1;
					SetDog(WIDTH, player.y, -50, 0, 0, imgDog, 10);
					SetDog(WIDTH, GetRand(1000)-500, -50, 0, 0, imgDog, 10);
					SetDog(WIDTH, GetRand(1000)-500, -50, 0, 0, imgDog, 10);
					SetDog(WIDTH, GetRand(1000)-500, -50, 0, 0, imgDog, 10);
					SetDog(WIDTH, GetRand(1000)-500, -50, 0, 0, imgDog, 10);
					strcpy_s(effectText, "ドッグフードを食べてしまった！移動が困難になり、犬が怒ってしまった...");
					effectColor = GetColor(255, 0, 0);
					effectTimer = FPS * 2;
				}
				noDamageFrame = FPS/2;
				BadFood[i].state = 0;
			}
		}
	}
}

int SetGoodFood(int x, int y, int vx, int vy, int ptn, int img, int hp)
{
	for (int i = 0; i < GOODFOOD_MAX; i++) {
		if (GoodFood[i].state == 0) {
			GoodFood[i].x = x;
			GoodFood[i].y = y;
			GoodFood[i].vx = vx;
			GoodFood[i].vy = vy;
			GoodFood[i].state = 1;
			GoodFood[i].pattern = ptn;
			GoodFood[i].image = img;
			GoodFood[i].hp = hp;
			GetGraphSize(img, &GoodFood[i].wid, &GoodFood[i].hei);
			return i;
		}
	}
	return -1;
}

void MoveGoodFood(void)
{
	for (int i = 0; i < GOODFOOD_MAX; i++) {
		if (GoodFood[i].state == 0) { continue; }
		GoodFood[i].x += GoodFood[i].vx;
		GoodFood[i].y += GoodFood[i].vy;
		DrawImage(GoodFood[i].image, GoodFood[i].x, GoodFood[i].y);
		if (GoodFood[i].x < -200 || WIDTH + 200 < GoodFood[i].x || GoodFood[i].y < -200 || HEIGHT + 200 < GoodFood[i].y) { GoodFood[i].state = 0; }
		if (GoodFood[i].hp > 0)
		{
			for (int j = 0; j < ATTACK_MAX; j++) {
				if (attack[j].state == 0) { continue; }
				int dx = abs((int)(GoodFood[i].x - attack[j].x));
				int dy = abs((int)(GoodFood[i].y - attack[j].y));
				if (dx < GoodFood[i].wid / 2 + attack[j].wid / 2 && dy < GoodFood[i].hei + attack[j].hei / 2) {
					DamageGoodFood(i, 1);
				}
			}
		}
		int dx = abs((int)(GoodFood[i].x - player.x));
		int dy = abs((int)(GoodFood[i].y - player.y));
		if (dx < GoodFood[i].wid / 2 + player.wid / 2 && dy < GoodFood[i].hei /2 + player.hei / 2) {
			if (player.hp > 0) {
				if (GoodFood[i].pattern == VEGETABLES) {
					player.hp += 20;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					player.vx += 1;
					if (player.vx > 30)
						player.vx = 30;
					strcpy_s(effectText, "野菜を食べた！横の移動がはやくなった！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;
				}
				if (GoodFood[i].pattern == WATERMELON) {
					player.hp += 20;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					player.vy += 1;
					if (player.vy >30)
						player.vy = 30;
					strcpy_s(effectText, "スイカを食べた！縦の移動がはやくなった！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;

				}
				if (GoodFood[i].pattern == APPLE) {
					player.hp += 40;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					strcpy_s(effectText, "リンゴを食べた！他よりHPが少し多く回復したった！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;
				}
				if (GoodFood[i].pattern == BANANA) {
					player.hp += 60;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					strcpy_s(effectText, "バナナを食べた！他よりHPが多く回復した！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;
				}
				if (GoodFood[i].pattern == COOKED_FISH) {
					player.hp += 20;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					player.vx += 2;
					if (player.vx > 30)
						player.vx = 30;
					player.vy += 2;
					if (player.vy > 30)
						player.vy = 30;
					strcpy_s(effectText, "焼き魚を食べた！縦横の移動がさらにはやくなった！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;
				}
				if (GoodFood[i].pattern == CAT_MILK) {
					player.hp += 20;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					milkCount--;
					if (milkCount < 0)
						milkCount = 0;
					coffeeCount--;
					if (coffeeCount < 0)
						coffeeCount = 0;
					reverseTimer = 0;
					player.vx += 2;
					if (player.vx > 30)
						player.vx = 30;
					player.vy += 2;
					if (player.vy > 30)
						player.vy = 30;
					strcpy_s(effectText, "猫用の牛乳を飲んだ！全ての状態異常が軽減され、縦横の移動がはやくなった！");
					effectColor = GetColor(0, 255, 0);
					effectTimer = FPS * 2;
				}
				if (GoodFood[i].pattern == CAT_FOOD) {
					player.hp += 200;
					if (player.hp > PLAYER_HP_MAX)
						player.hp = PLAYER_HP_MAX;
					player.vx += 5;
					if (player.vx > 30)
						player.vx = 30;
					player.vy += 5;
					if (player.vy > 30)
						player.vy = 30;
					strcpy_s(effectText, "キャットフードを食べた！HPが大幅に回復し、縦横の移動が非常にはやくなった！");
					effectColor = GetColor(0,255,  0);
					effectTimer = FPS * 2;
				}
				GoodFood[i].state = 0;
			}
		}
	}
}

int SetDog(int x, int y, int vx, int vy, int ptn, int img, int hp)
{
	for (int i = 0; i < DOG_MAX; i++) {
		if (Dog[i].state == 0) {
			Dog[i].x = x;
			Dog[i].y = y;
			Dog[i].vx = vx;
			Dog[i].vy = vy;
			Dog[i].state = 1;
			Dog[i].pattern = ptn;
			Dog[i].image = img;
			Dog[i].hp = hp;
			GetGraphSize(img, &Dog[i].wid, &Dog[i].hei);
			return i;
		}
	}
	return -1;
}

void MoveDog(void)
{
	for (int i = 0; i < DOG_MAX; i++) {
		if (Dog[i].state == 0) { continue; }
		Dog[i].x += Dog[i].vx;
		Dog[i].y += Dog[i].vy;
		DrawImage(Dog[i].image, Dog[i].x, Dog[i].y);
		if (Dog[i].x < -200 || WIDTH + 200 < Dog[i].x || Dog[i].y < -200 || HEIGHT + 200 < Dog[i].y) { Dog[i].state = 0; }
		if (Dog[i].hp > 0)
		{
			for (int j = 0; j < ATTACK_MAX; j++) {
				if (attack[j].state == 0) { continue; }
				int dx = abs((int)(Dog[i].x - attack[j].x));
				int dy = abs((int)(Dog[i].y - attack[j].y));
				if (dx < Dog[i].wid / 2 + attack[j].wid / 2 && dy < Dog[i].hei + attack[j].hei / 2) {
					DamageDog(i, 1);
				}
			}
		}
		if (noDamageFrame == 0)
		{
			int dx = abs((int)(Dog[i].x - player.x));
			int dy = abs((int)(Dog[i].y - player.y));
			if (dx < Dog[i].wid / 2 + player.wid / 2 && dy < Dog[i].hei/2 + player.hei / 2) {
				player.hp -= 100;
			}
		}
	}
}

void DamageBadFood(int n, int dmg)
{
	BadFood[n].hp -= dmg;
	if (BadFood[n].hp <= 0) {
		BadFood[n].state = 0;
	}
}

void DamageGoodFood(int n, int dmg)
{
	GoodFood[n].hp -= dmg;
	if (GoodFood[n].hp <= 0) {
		GoodFood[n].state = 0;
	}
}

void DamageDog(int n, int dmg)
{
	Dog[n].hp -= dmg;
	if (Dog[n].hp <= 0) {
		Dog[n].state = 0;
	}
}

void DrawText(int x, int y, const char* txt, int val, int col, int siz)
{
	SetFontSize(siz);
	DrawFormatString(x + 1, y + 1, 0x000000, txt, val);
	DrawFormatString(x, y, col, txt, val);
}

void DrawParameter(void)
{
	int x = 10, y = HEIGHT - 30;
	DrawBox(x, y, x + PLAYER_HP_MAX, y + 20, 0x000000, TRUE);
	for (int i = 0; i < player.hp; i++)
	{
		DrawBox(x+1 + i, y + 2, x + i, y + 18, 0x00ff00, TRUE);
	}
}

// 画像の読み込み、読み込み失敗時は通知
int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "画像の読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}