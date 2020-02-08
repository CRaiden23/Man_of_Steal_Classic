#include "UI.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Player.h"
#include "helpers.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.

//CUI::CUI() :CUI((eSpriteType)0, Vector2::Zero) {
//}

CUI::CUI(eSpriteType t, const Vector2& p):CObject(t, p) {
	score = 0;
	lastHealth = 3;
	tiny = 0;
	big = 0;
	m_nCurrentFrame = 0;
	if (t == NVISION_SPRITE)
	{
		m_nCurrentFrame = 2;
	}
	vision = 1;
	batteryLife = 1000;
	getFile();
	initials[0] = '-';
	initials[1] = '-';
	initials[2] = '-';
} //constructor

void CUI::usingDec()
{
	batteryLife -= 50;
}

void CUI::setDif(char set)
{
	difficulty = set;
}

void CUI::writeFile()
{
	ofstream onfile;
	onfile.open("score.sav");
	for (int i = 0; i < storage.size(); i++)
	{
		onfile << storage[i].init[0];
		onfile << storage[i].init[1];
		onfile << storage[i].init[2];
		onfile << " ";
		onfile << storage[i].score;
		onfile << " ";
		onfile << storage[i].dif;
		onfile << "\n";
	}
	onfile.close();
}

void CUI::storeSort()
{
	char tempInit[3];
	m_pPOINTER->getInitials(tempInit);
	if (m_pSCORE->getScore() > storage[9].score)
	{
		storage[9].dif = m_pPOINTER->getDif();
		storage[9].score = m_pSCORE->getScore();
		storage[9].init[0] = tempInit[0];
		storage[9].init[1] = tempInit[1];
		storage[9].init[2] = tempInit[2];
	}
	int temp;
	char tempDif;
	// This for loop sorts for score
	for (int i = 0; i < storage.size(); i++)
	{
		for (int j = 0; j < storage.size(); j++)
		{
			if (storage[i].score > storage[j].score)
			{
				tempDif = storage[i].dif;
				temp = storage[i].score;
				tempInit[0] = storage[i].init[0];
				tempInit[1] = storage[i].init[1];
				tempInit[2] = storage[i].init[2];

				storage[i].dif = storage[j].dif;
				storage[i].score = storage[j].score;
				storage[i].init[0] = storage[j].init[0];
				storage[i].init[1] = storage[j].init[1];
				storage[i].init[2] = storage[j].init[2];

				storage[j].dif = tempDif;
				storage[j].score = temp;
				storage[j].init[0] = tempInit[0];
				storage[j].init[1] = tempInit[1];
				storage[j].init[2] = tempInit[2];
			}
		}
	}
	// This for loop sorts for difficulty
	for (int i = 0; i < storage.size(); i++)
	{
		for (int j = 0; j < storage.size(); j++)
		{
			if ((storage[i].dif == 'M' && storage[j].dif != 'M') || (storage[i].dif == 'H' && storage[j].dif == 'E'))
			{
				tempDif = storage[i].dif;
				temp = storage[i].score;
				tempInit[0] = storage[i].init[0];
				tempInit[1] = storage[i].init[1];
				tempInit[2] = storage[i].init[2];

				storage[i].dif = storage[j].dif;
				storage[i].score = storage[j].score;
				storage[i].init[0] = storage[j].init[0];
				storage[i].init[1] = storage[j].init[1];
				storage[i].init[2] = storage[j].init[2];

				storage[j].dif = tempDif;
				storage[j].score = temp;
				storage[j].init[0] = tempInit[0];
				storage[j].init[1] = tempInit[1];
				storage[j].init[2] = tempInit[2];
			}
		}
	}
	//sorts by difficulty and score
	for (int i = 0; i < storage.size(); i++)
	{
		for (int j = 0; j < storage.size(); j++)
		{
			if (storage[i].dif == storage[j].dif && storage[i].score > storage[j].score)
			{
				tempDif = storage[i].dif;
				temp = storage[i].score;
				tempInit[0] = storage[i].init[0];
				tempInit[1] = storage[i].init[1];
				tempInit[2] = storage[i].init[2];

				storage[i].dif = storage[j].dif;
				storage[i].score = storage[j].score;
				storage[i].init[0] = storage[j].init[0];
				storage[i].init[1] = storage[j].init[1];
				storage[i].init[2] = storage[j].init[2];

				storage[j].dif = tempDif;
				storage[j].score = temp;
				storage[j].init[0] = tempInit[0];
				storage[j].init[1] = tempInit[1];
				storage[j].init[2] = tempInit[2];
			}
		}
	}
}

void CUI::changeInitials(int index, int direction)
{
	if (initials[index] == '-' && direction == 1)
	{
		direction = 20;
		initials[index] += direction;
	}
	else if (initials[index] == '-' && direction == -1)
	{
		direction = 45;
		initials[index] += direction;
	}
	else if (initials[index] == 'A' && direction == -1)
	{
		direction = -20;
		initials[index] += direction;
	}
	else if (initials[index] == 'Z' && direction == 1)
	{
		direction = -45;
		initials[index] += direction;
	}
	else
		initials[index] += direction;
}


void CUI::getInitials(char *temps)
{
	temps[0] = initials[0];
	temps[1] = initials[1];
	temps[2] = initials[2];
}

void CUI::changePos(Vector2 &newPos)
{
	m_vPos = newPos;
	//update bounding shapes

	CObject::move();
}

void CUI::setScore(int temp)
{
	score = temp;
}

void CUI::addScore(int add)
{
	score += add;
	if (score > 9999)
	{
		score = 9999;
	}
}

void CUI::setVisionD()
{
	vision = 1;
}

void CUI::setVisionL()
{
	vision = 2;
}

void CUI::changeVision()
{
	vision *= -1;
}

void CUI::incremB()
{
	batteryLife++;
}
void CUI::decreaseB()
{
	batteryLife--;
}

void CUI::increaseB()
{
	batteryLife += 400;
}

void CUI::setBattery(int temp)
{
	batteryLife = temp;
}

void CUI::getFile()
{
	ifstream infile;
	infor obj;
	infile.open("score.sav");
	if (infile.is_open())
	{
		while (!(infile.eof()))
		{
			infile >> obj.init[0];
			infile >> obj.init[1];
			infile >> obj.init[2];
			infile >> obj.score;
			infile >> obj.dif;
			storage.push_back(obj);
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			obj.init[0] = '-';
			obj.init[1] = '-';
			obj.init[2] = '-';
			obj.score = 0;
			obj.dif = 'E';
			storage.push_back(obj);
		}
		writeFile();
	}
	storage.resize(10);
	infile.close();
}

void CUI::Update()
{	
	//printf("Difficulty: %c\n", m_pPOINTER->getDif());
	if (m_nSpriteIndex == MOVEUI_SPRITE)
	{
		const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));
		float player_speed = clamp(m_pPlayer->getPSpeed() * sqrt(pow(m_pPlayer->GetVelocity().x, 2) + pow(m_pPlayer->GetVelocity().y, 2)), -m_pPlayer->getPSpeed(), m_pPlayer->getPSpeed());
		if (m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt)// if we have multiple frames and it's time to update it's frame
		{
			if (player_speed == 0.0f)// if our character is idle
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 0;
			}
			else if (player_speed == 75.0f) // if our character is walking
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 3;
				//m_pSCORE->addScore(10);
			}
			else if (player_speed > 75.0f) // if our character is running
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 1;
			}
			else if (player_speed < 75.0f && player_speed > 0.0f) // if our character is crawling
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 2;
			}
			else
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 0;
			}

		} //if
	} //if

	else if (m_nSpriteIndex == BATTERY_SPRITE)
	{
		//printf("Battery: %d\n", m_pBATTERY->getBattery());
		if (m_pBATTERY->getBattery() >= 1001)
		{
			m_nCurrentFrame = 6;
		}
		else if (m_pBATTERY->getBattery() >= 810)
		{
			m_nCurrentFrame = 0;
		}
		else if (m_pBATTERY->getBattery() >= 610)
		{
			m_nCurrentFrame = 1;
		}
		else if (m_pBATTERY->getBattery() >= 410)
		{
			m_nCurrentFrame = 2;
		}
		else if (m_pBATTERY->getBattery() >= 210)
		{
			m_nCurrentFrame = 3;
		}
		else if (m_pBATTERY->getBattery() > 0)
		{
			m_nCurrentFrame = 4;
		}
		else
		{
			m_nCurrentFrame = 5;
			if (m_pVISION->getVision() == -2)
			{
				m_pVISION->setVisionL();
				m_pAudio->play(NIGHTV2_SOUND);
			}
			else if (m_pVISION->getVision() == -1)
			{
				m_pVISION->setVisionD();
				m_pAudio->play(NIGHTV2_SOUND);
			}
			m_pBATTERY->setBattery(0);
		}
	}

	else if (m_nSpriteIndex == NVISION_SPRITE ||
			 m_nSpriteIndex == NVGSCREEN_SPRITE)
	{
		if (m_pVISION->getVision() == 1)
		{
			this->m_nSpriteIndex = NVISION_SPRITE;
			m_nCurrentFrame = 2; // dark room
		}
		else if (m_pVISION->getVision() == 2)
		{
			this->m_nSpriteIndex = NVISION_SPRITE;
			m_nCurrentFrame = 0; // light room
			if(m_pBATTERY->getBattery() < 810 && m_pStepTimer->GetFrameCount() % 15 == 0)
				m_pBATTERY->incremB();
		}
		else if (m_pVISION->getVision() == -2)
		{
			this->m_nSpriteIndex = NVISION_SPRITE;
			m_nCurrentFrame = 3; // too bright room
			m_pBATTERY->decreaseB();
		}
		else
		{
			this->m_nSpriteIndex = NVGSCREEN_SPRITE;

			//Borrowed from CObject::animator() to get the sprite to animate proper
			const size_t nFrameCount = m_pRenderer->GetNumFrames(m_nSpriteIndex);

			const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));

			if (nFrameCount > 1 && m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt)
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = (m_nCurrentFrame + 1) % nFrameCount;
			}
			m_pBATTERY->decreaseB();
		}
		//printf("Battery Life: %d\n",m_pBATTERY->getBattery());
	} // else if
	
	else if (m_nSpriteIndex == HPUI_SPRITE)
	{
		lastHealth = m_pPlayer->getHealth();
		const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));
		if (m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt)// if we have multiple frames and it's time to update it's frame
		{
			if (lastHealth == 3)// if our player's hp is 3
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 0;
			}
			else if (lastHealth == 2) // if our player's hp is 2
			{
				if (fmod(m_pStepTimer->GetTotalSeconds(), 4.0f) < 1.0f)
					this->m_f4Tint.x = 0.0f;
				else
					this->m_f4Tint.x = 1.0f;

				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 1;
			}
			else if (lastHealth == 1) // if our player's hp is 1
			{
				if(fmod(m_pStepTimer->GetTotalSeconds(), 2.0f) < 1.0f)
					this->m_f4Tint.x = 0.0f;
				else
					this->m_f4Tint.x = 1.0f;

				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 2;
			}
			else if (lastHealth <= 0) // if our player's hp is 0
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 3;
			}
			//printf("Player Last Health: %d\n", lastHealth);
		} // if
	} //else if
	else if (m_nSpriteIndex == NUMA_SPRITE)
	{
		if (m_pSCORE->getScore() > 0)
		{
			big = m_pSCORE->getScore();
			tiny = big % 10;
			big -= tiny;
			big /= 10;
			m_nCurrentFrame = tiny;
		}
		else
		{
			m_nCurrentFrame = 0;
		}
	}
	else if (m_nSpriteIndex == NUMB_SPRITE)
	{
		if (m_pSCORE->getScore() >= 10)
		{
			big = m_pSCORE->getScore();
			for (int i = 0; i < 2; i++)
			{
				tiny = big % 10;
				big -= tiny;
				if (big != 0)
				{
					big /= 10;
				}
			}
			m_nCurrentFrame = tiny;
		}
		else
		{
			m_nCurrentFrame = 0;
		}
	}
	else if (m_nSpriteIndex == NUMC_SPRITE)
	{
		if (m_pSCORE->getScore() >= 100)
		{
			big = m_pSCORE->getScore();
			for (int i = 0; i < 3; i++)
			{
				tiny = big % 10;
				big -= tiny;
				if (big != 0)
				{
					big /= 10;
				}
			}
			m_nCurrentFrame = tiny;
		}
		else
		{
			m_nCurrentFrame = 0;
		}
	}
	else if (m_nSpriteIndex == NUMD_SPRITE)
	{
		if (m_pSCORE->getScore() >= 1000)
		{
			big = m_pSCORE->getScore();
			for (int i = 0; i < 4; i++)
			{
				tiny = big % 10;
				big -= tiny;
				if (big != 0)
				{
					big /= 10;
				}
			}
			m_nCurrentFrame = tiny;
		}
		else
		{
			m_nCurrentFrame = 0;
		}
	}
}