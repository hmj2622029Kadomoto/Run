#pragma once

// 構造体の宣言
struct OBJECT // 自分や敵用
{
	int x;
	int y;
	int vx;
	int vy;
	int state;
	int pattern;
	int image;
	int wid;
	int hei;
	int hp;
	int timer;
};


// 関数プロトタイプ宣言
void InitGame(void);
void ScrollBG(int spd);
void InitVariable(void);
void DrawImage(int img, int x, int y);
void MovePlayer(void);
void SetBullet(void);
void MoveBullet(void);
int SetEnemy(int x, int y, int vx, int vy, int ptn, int img, int sld);
void MoveEnemy(void);
void StageMap(void);
void DamageEnemy(int n, int dmg);
void DrawText(int x, int y, const char* txt, int val, int col, int siz);
void DrawParameter(void);
void SetItem(void);
void MoveItem(void);
void DrawTextC(int x, int y, const char* txt, int col, int siz);



int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);