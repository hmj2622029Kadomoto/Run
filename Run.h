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
void SetAttack(void);
void MoveAttack(void);
int SetBadFood(int x, int y, int vx, int vy, int ptn, int img, int hp);
int SetGoodFood(int x, int y, int vx, int vy, int ptn, int img, int hp);
void MoveBadFood(void);
void MoveGoodFood(void);
int SetDog(int x, int y, int vx, int vy, int ptn, int img, int hp);
void MoveDog(void);
int SetHome(int x, int y, int vx, int vy, int img);
void MoveHome(void);
void StageMap(void);
void DamageBadFood(int n, int dmg);
void DamageGoodFood(int n, int dmg);
void DamageDog(int n, int dmg);
void DrawText(int x, int y, const char* txt, int val, int col, int siz);
void DrawParameter(void);
void DrawTextC(int x, int y, const char* txt, int col, int siz);


int HealByStage(int heal);
int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);