#pragma once


class DifficultyManager
{
public:
	void Update(float dt)
	{
		m_timeFromStart += dt;
		m_difficultyValue = 1.0f + (m_timeFromStart / m_difficultyScaling); // +1 every 5 mins
		//Future changes may need difficulty settings that could be passed in constructor
	}

	const float& getDifficultyValue() const { return m_difficultyValue; }
	
	const float& getCreditsperSecond() const { return m_difficultyValue * m_baseCreditsGain * 5.0f; }

	//Potential elite-like buff chance

	DifficultyManager(float baseCredits, float difficultyScaling)
		: m_baseCreditsGain(baseCredits),m_difficultyScaling(difficultyScaling*60.0f)
	{}
private:
	float m_timeFromStart = 0.0f;
	float m_difficultyValue = 0.0f;
	float m_baseCreditsGain;
	float m_difficultyScaling; // In secconds
};