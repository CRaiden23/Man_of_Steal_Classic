#include "Music.h"

CSoundDesc CMusic::loop(int i, float volume)
{
	CSoundDesc desc;

	if (i < 0 || i >= m_nCount || m_bMuted || m_bPlayed[i]) //if bad index, or muted, or already started
		return desc; //bail out

	desc.m_nEffectIndex = i;
	const int instance = getNextInstance(i); //instance of sound

	if (instance >= 0 && instance < m_nInstanceCount[i]) { //if unused copy found
		desc.m_nInstanceIndex = instance;
		m_pInstance[i][instance]->Play(true); //play looped
		m_bPlayed[i] = true;

		//set volume
		const float v = max(0.0f, min(volume, 1.0f)); //volume
		m_pInstance[i][instance]->SetVolume(v);
	} //if

	return desc;
}


//Takes in a sound descriptor and a float as parameters
//Returns a bool if fading has stopped the music
bool CMusic::Fade(const CSoundDesc& d, float newVol)
{
	const int i = d.m_nEffectIndex;

	if (i < 0 || i >= m_nCount) 
		return false;

	const int j = d.m_nInstanceIndex;

	if (j < 0 || j >= m_nInstanceCount[i]) 
		return false;

	//Set the volume if it's greater than 0.01f;
	if (newVol > 0.01f)
	{
		m_pInstance[i][j]->SetVolume(newVol);

		return false;
	}
	else
		m_pInstance[i][j]->Stop();

	return true;
}
