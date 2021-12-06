#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
using namespace std;

#define numOfCard 40

ScenePtr scene1, scene2, scene3, scene4;
ObjectPtr start, back, randomcard, nextbtn, endbtn, restart, help, ban, unobtn, keptComCard[14], hide;
TimerPtr banTimer, unoTimer;
SoundPtr bgm, unoeffect, cardshare, cardslide, win, lose;

ObjectPtr mycard[14], comcard[14];
ObjectPtr stdCard, randomCard[25]; //���� ī��, ����ī�����
int stdnum, randomnum[25]; //���� ī��, ����ī�尡 ����Ű�� Ŭ���� �迭�� �ѹ�
int myCardnum[14], comCardnum[14]; //���� ī�尡 ����Ű�� Ŭ���� �迭�� �ѹ�
int comNull = 7, myNull = 7; //���� ī���� ����, mycard[]�� comcard[]�� ù �� ���� �ǹ�.
int mixCard[numOfCard]; //���� ���� ī�� �迭
int nextCard = 15; //������ ������ ī�� �ѹ�

int seledtedCardnum[20]; //Ŭ���� ī�� �ѹ� ����
int takeCardCount = 0; //�÷��̾ ī�带 �� Ƚ��
int keepCardCount = 0; //�÷��̾ ī�带 ������ Ƚ��

bool tookCard = 0; //�÷��̾ ī�带 �´��� -> �� ������ 0
bool keptCard = 0; //�÷��̾ ī�带 �����Դ��� -> �� ���������� 0
bool turn = 0; //�÷��̾� �� = 0, ��ǻ�� �� = 1
bool uno = false;

enum COL { red, green, blue, yellow };
class card { //ī�� Ŭ����: ����, ����, �̹��� ������Ʈ�����͸� ����� ����
public:
	int num;
	COL color;
	ObjectPtr cardObject;

	card() {};
};
card allCard[numOfCard]; //��� ī�� Ŭ���� �迭 ����

void init_game(); //���� �ʱ� ����
void setClassMem(); //Ŭ���� ��� �����Լ�
void random(int card[numOfCard]); //������ �� ī�弯��

void random_card();
void play_game();
void com_play(); //��ǻ�� �ڵ� �÷���
void my_play(); //�÷��̾� �÷���
void end_game(); //���� ���� ȭ��
void ban_card();
void press_uno();
void keepCard();

int main()
{
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

	bgm = Sound::create("sounds/bgm.mp3");
	unoeffect = Sound::create("sounds/uno.mp3");
	cardshare = Sound::create("sounds/cardshare.mp3");
	cardslide = Sound::create("sounds/cardslide.mp3");
	win = Sound::create("sounds/win.mp3");
	lose = Sound::create("sounds/lose.mp3");
	bgm->play(true);

	scene1 = Scene::create(" ", "images/start.png"); // ����ȭ��
	scene2 = Scene::create(" ", "images/play.png");
	scene3 = Scene::create(" ", "images/rule.png");

	start = Object::create("images/startbtn.png", scene1, 720, 100); // ���۹�ư
	help = Object::create("images/helpbtn.png", scene1, 415, 100); //�����ư

	start->setScale(0.6f);
	help->setScale(0.65f);

	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene2->enter();
		return true;
		});

	//���ӷ� ���� ������
	help->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene3->enter();
		start->locate(scene3, 1050, 550);
		return true;
		});

	init_game();
	startGame(scene1);

	return 0;
}

void init_game() {

	back = Object::create("images/0.png", scene2, 600, 270); // ó���� ī�� �޸� ������ ī�� ������
	randomcard = Object::create("images/0.png", scene2, 1100, 270, false); // �̰� ������ ī�带 ��
	for (int i = 0; i < 14; i++) {
		keptComCard[i] = Object::create("images/0.png", scene2, 0, 0, false);
	}

	setClassMem();

	back->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		cardshare->play();
		play_game();
		back->hide();
		randomcard->show();
		return true;
		});

}


void setClassMem() {
	for (int i = 0; i < numOfCard; i++) { //��� num ����
		if (i < 10) allCard[i].color = red;
		else if (10 <= i && i < 20) allCard[i].color = yellow;
		else if (19 <= i && i < 30) allCard[i].color = green;
		else allCard[i].color = blue;
	}
	for (int i = 0; i < numOfCard; i++) { //��� color ����
		switch (i % 10) {
		case 0:
			allCard[i].num = 0;
			break;
		case 1:
			allCard[i].num = 1;
			break;
		case 2:
			allCard[i].num = 2;
			break;
		case 3:
			allCard[i].num = 3;
			break;
		case 4:
			allCard[i].num = 4;
			break;
		case 5:
			allCard[i].num = 5;
			break;
		case 6:
			allCard[i].num = 6;
			break;
		case 7:
			allCard[i].num = 7;
			break;
		case 8:
			allCard[i].num = 8;
			break;
		case 9:
			allCard[i].num = 9;
			break;
		}
	}
	char path[numOfCard]; //��� allcard ����
	for (int i = 0; i < numOfCard; i++) {
		sprintf(path, "images/%d.png", i + 1);
		allCard[i].cardObject = Object::create(path, scene2, 0, 0, false);
		allCard[i].cardObject->setScale(0.8f);
	}
}

int index_to_x(int who, int index) { //ī���� x ��ġ ���� �Լ�
	return 150 + 150 * index;
}

void random_card()
{
	random(mixCard); //mixCard[40]�� ���� ���� 40�� ����, 0~6:mycard, 7~13:comcard, 14:ù��° stdcard

	for (int i = 0; i < 40; i++) {
		printf("mixcard[%d] = %d ", i, mixCard[i]);
	}
	printf("\n");
	for (int i = 0; i < 7; i++)
	{
		myCardnum[i] = mixCard[i]; //mixCard�� i��° �������ڸ� myCardnum[i]�� ����
		mycard[i] = allCard[myCardnum[i]].cardObject; //myplay��° ī�尴ü�� Object�� mycard�迭 i��°�� ����
		mycard[i]->locate(scene2, index_to_x(1, i), 140); //������ ��ü����� ��ġ ����
		mycard[i]->show(); //��ü��� ���̱�

		comCardnum[i] = mixCard[i + 7]; //mixCard�� i��° �������ڸ� comCardnum[i]�� ����

		keptComCard[i]->locate(scene2, index_to_x(0, i), 500);
		keptComCard[i]->show();

	}

	stdnum = mixCard[14]; //ù��° ����ī��
	stdCard = allCard[stdnum].cardObject;
	stdCard->locate(scene2, 600, 310);
	stdCard->show();
	printf("stdnum = %d ", stdnum);
	printf("\n");
	for (int i = 0; i < 25; i++) { //����ī�����
		randomnum[i] = mixCard[15 + i];
		randomCard[i] = allCard[randomnum[i]].cardObject;
		printf("randomnum[%d] = %d ", i, randomnum[i]);
		printf("\n");
	}
}

void random(int card[numOfCard]) { // ù ī�� ������
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 40; i++) {
		//������ �� ����
		card[i] = rand() % numOfCard;
		// ���� ���� -- ī�� 7�� �� ������
		for (int sub_i = 0; sub_i < i; sub_i++)
		{
			if (card[i] == card[sub_i])
			{
				i--;
				break;
			}
		}
	}

}

void play_game()
{

	random_card();
	showMessage("������ �÷����մϴ�.");

	my_play();

}

void whosTurn() {

	if (turn == 0) my_play();
	else com_play();

}
void locateKeepCard(int num) {
	myCardnum[num] = randomnum[nextCard - 15];
	mycard[num] = allCard[myCardnum[num]].cardObject; //myplay��° ī�尴ü�� Object�� mycard�迭 i��°�� ����

	if (num < 7)
		mycard[num]->locate(scene2, index_to_x(1, num), 140);
	else
		mycard[num]->locate(scene2, 225 + 150 * (num - 7), 20);

	mycard[num]->setScale(0.8f);
	mycard[num]->show();
}
void keepCard() {		//�÷��̾�: ī�� ��������
	if (takeCardCount > keepCardCount) {
		locateKeepCard(seledtedCardnum[keepCardCount]);
	}
	else {
		locateKeepCard(myNull);
	}

	keptCard = 1;
	keepCardCount++;
	myNull++;
	nextCard++;

	turn = 1;
	whosTurn();
	//		com_play();
}

void my_play() {

	nextbtn = Object::create("images/next.png", scene2, 200, 280, false);
	randomcard->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		if (myNull == 14) {
			bgm->stop();
			lose->play();
			showMessage("You Lose..");
			end_game();
		}

		else if (tookCard == 0) keepCard();	//������ ī�带 ���� �ʾ����� ī�� ��������

		for (int i = 0; i < 7 + keepCardCount; i++) {
			printf("myCardnum[%d] = %d ", i, myCardnum[i]);
			printf("\n");
		}

		return true;
		});


	for (int i = 0; i < myNull; i++) {
		mycard[i]->setOnMouseCallback([&, i](auto, auto, auto, auto)->bool {
			if (allCard[stdnum].num == allCard[myCardnum[i]].num || allCard[stdnum].color == allCard[myCardnum[i]].color)
			{
				stdCard->hide();
				stdnum = myCardnum[i];
				stdCard = allCard[stdnum].cardObject;   // ���� ī��� �ٲٱ�
				stdCard->locate(scene2, 600, 310);
				stdCard->setScale(0.8f);
				stdCard->show();
				nextbtn->show();
				printf("changed stdnum = %d", stdnum);
				printf("\n");
				myNull--;
				tookCard = 1;

				seledtedCardnum[takeCardCount] = i;	//������ ī���� mycard �迭 �ѹ��� ���� 
				takeCardCount++;

				for (int i = 0; i < 7 + keepCardCount; i++) {
					printf("myCardnum[%d] = %d ", i, myCardnum[i]);
					printf("\n");
				}

				if (myNull == 1) {		//���� ������ ��� ��ħ
					uno = true;
					press_uno();
				}
				else if (myNull == 0) {	//ī�带 ��� ���� �̱�
					bgm->stop();
					win->play();
					showMessage("You Win!!");
					end_game();
				}
			}

			else ban_card();

			return true;
			});
	}
	for (int i = 7; i < keepCardCount; i++) {
		mycard[i]->setOnMouseCallback([&, i](auto, auto, auto, auto)->bool {
			if (allCard[stdnum].num == allCard[myCardnum[i]].num || allCard[stdnum].color == allCard[myCardnum[i]].color)
			{
				stdCard->hide();
				stdnum = myCardnum[i];
				stdCard = allCard[stdnum].cardObject;   // ���� ī��� �ٲٱ�
				stdCard->locate(scene2, 600, 310);
				stdCard->setScale(0.8f);
				stdCard->show();
				nextbtn->show();
				printf("changed stdnum = %d", stdnum);
				printf("\n");
				myNull--;
				tookCard = 1;

				seledtedCardnum[takeCardCount] = i;	//������ ī���� mycard �迭 �ѹ��� ���� 
				takeCardCount++;

				for (int i = 0; i < 7 + keepCardCount; i++) {
					printf("myCardnum[%d] = %d ", i, myCardnum[i]);
					printf("\n");
				}

				if (myNull == 1) {		//���� ������ ��� ��ħ
					uno = true;
					press_uno();
				}
				else if (myNull == 0) {	//ī�带 ��� ���� �̱�
					bgm->stop();
					win->play();
					showMessage("You Win!!");
					end_game();
				}
			}

			else ban_card();

			return true;
			});
	}

	nextbtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		hideTimer();
		nextbtn->hide();

		turn = 1;
		whosTurn();
		//		com_play();

		return true;
		});

}

void com_play() {
	int T = 0;
	for (int i = 0; i < comNull; i++) { //���� ���ڰ� ��� �ٸ���
		if (allCard[stdnum].num != allCard[comCardnum[i]].num && allCard[stdnum].color != allCard[comCardnum[i]].color)
			T++;
	}
	if (T == comNull) { //���� ��������.
		if (comNull == 14) { //14�� �Ѿ�� ��������
			bgm->stop();
			win->play();

			showMessage("You Win!!");
			end_game();
		}
		else {
			cardslide->play();
			comCardnum[comNull] = randomnum[nextCard - 15];
			comcard[comNull] = allCard[comCardnum[comNull]].cardObject;	//ī�� ���̿��� ���� ������

			if (comNull < 7) keptComCard[comNull]->locate(scene2, 150 + 150 * comNull, 500);
			else keptComCard[comNull]->locate(scene2, 225 + 150 * (comNull - 7), 540);
			keptComCard[comNull]->show();

			showMessage("��ǻ�Ͱ� ���� ���������ϴ�.");

			comNull++;
			nextCard++; //comcard�� ������ ���� ������ ī�� �ѹ� 1�� ����
			tookCard = 0;
			keptCard = 0;

			turn = 0;
			whosTurn();
			my_play();
		}
	}
	else {
		int count = 0;			//��ǻ�Ͱ� �� ī���� ����

		for (int i = 0; i < comNull; i++) { //���� ���� �� �ϳ��� ������
			if (allCard[stdnum].num == allCard[comCardnum[i]].num || allCard[stdnum].color == allCard[comCardnum[i]].color) {

				cardslide->play();
				stdCard->hide();
				stdnum = comCardnum[i];
				stdCard = allCard[stdnum].cardObject;
				stdCard->locate(scene2, 600, 310);
				stdCard->show();

				keptComCard[comNull - 1]->hide();

				comNull--;								//comcard�� ���� 1 ����
				count++;

				for (int j = 0; j < comNull - i; j++) {
					comCardnum[i + j] = comCardnum[i + j + 1];
					comcard[i + j] = allCard[myCardnum[i + j]].cardObject;
				}

				if (comNull == 1) {		//���� ������ ��� ��ħ
					unoeffect->play();
				}
			}
		}
		char countMessage[50];
		sprintf(countMessage, "��ǻ�Ͱ� ī�带 %d�� �½��ϴ�.", count);

		if (comNull == 0) {
			bgm->stop();
			lose->play();

			showMessage("You Lose..");
			end_game();
		}
		else showMessage(countMessage);

		count = 0;
		tookCard = 0;
		keptCard = 0;

		turn = 0;
		whosTurn();
		my_play();
	}

	T = 0;
}
//  --  ���ڳ� ���� �ٸ��� ban�̹��� 1�� ���� �������, ������ ����ī��� �ٲٱ�
void ban_card() {
	ban = Object::create("images/ban.png", scene2, 550, 150, false);
	banTimer = Timer::create(1.f);
	ban->setScale(0.1f);
	ban->show();
	banTimer->start();
	banTimer->setOnTimerCallback([&](TimerPtr)->bool {
		ban->hide();
		return true;
		});

}

void press_uno() {
	hide = Object::create("images/hide.png", scene2, 0, 0);
	unobtn = Object::create("images/unobtn.png", scene2, 1000, 20, false);
	unoTimer = Timer::create(3.f);
	unobtn->setScale(0.2f);
	unobtn->show();

	showTimer(unoTimer);
	unoTimer->start();      // 3�� �ȿ� ��� ��ư ������ ��


	unobtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		unoTimer->stop();
		hideTimer();
		unoTimer->set(3.f);
		unobtn->hide();
		unoeffect->play();
		uno = false;

		hide->hide();
		nextbtn->show();
		return true;
		});

	unoTimer->setOnTimerCallback([&](TimerPtr)->bool {
		if (uno == true) {    // 3�� ������ ��ư�� ������ ���� ����
			hideTimer();
			unoTimer->set(3.f);
			unobtn->hide();
			nextbtn->hide();

			cardslide->play();

			hide->hide();

			keepCard();

		}

		return true;
		});
}
void end_game() { //���� ���� ȭ��
	bgm->stop();
	nextbtn->hide();

	restart = Object::create("images/restartbtn.png", scene2, 370, 350); // �絵����ư
	endbtn = Object::create("images/endbtn.png", scene2, 720, 350); // �������ư
	restart->setScale(0.8f);
	endbtn->setScale(0.8f);

	endbtn->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		endGame();
		return true;
		});

	restart->setOnMouseCallback([&](auto, auto, auto, auto)->bool {

		scene1->enter();
		bgm->play();

		restart->hide();
		endbtn->hide();
		randomcard->hide();
		nextbtn->hide();

		if (myNull == 14)
		{
			for (int i = 0; i < myNull; i++) {
				mycard[i]->hide();
			}
		}
		else
		{
			for (int i = 0; i < myNull + keepCardCount; i++) {
				mycard[i]->hide();
			}
		}

		for (int i = 0; i < comNull; i++) {
			keptComCard[i]->hide();
		}
		stdCard->hide();

		comNull = 7;
		myNull = 7;
		nextCard = 15;

		back->show();

		return true;
		});
}