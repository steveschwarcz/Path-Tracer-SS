#pragma once
#include <random>
#include <ctime>


/// <summary>
/// Generate a uniform random distribution
/// </summary>
struct rng_uniform {
	rng_uniform() {
	}

	float operator()(float x, float y) { 
		std::uniform_real_distribution<float> dis(x, y);
		return dis(generator);
	}

private:
	std::default_random_engine generator;
};

class ProgressBar
{
private:
	std::clock_t progress_timer_start;
	int progress;
public:
	ProgressBar() : progress_timer_start(std::clock()), progress(-1) {};

	void updateProgress(int newProgress)
	{
			/*code for progess bar shamelessly stolen from http://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf */

			//check duration and that there was a change in progress
			if ((std::clock() - progress_timer_start) / static_cast<double>(CLOCKS_PER_SEC) > 1 &&
				newProgress > progress)
			{
				float progress = newProgress;
				int barWidth = 70;

				std::cout << "[";
				int pos = barWidth * static_cast<float>(progress) / 100;
				for (int i = 0; i < barWidth; ++i)
				{
					if (i < pos) std::cout << "=";
					else if (i == pos) std::cout << ">";
					else std::cout << " ";
				}
				std::cout << "] " << progress << " %\r";
				std::cout.flush();

				progress_timer_start = std::clock();
		}
	}

	void end()
	{
		std::cout.flush();
		std::cout << "                                                                                ";
		std::cout.flush();
	}
};