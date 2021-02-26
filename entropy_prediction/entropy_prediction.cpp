#include <iostream>
#include <fstream>
#include <direct.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

void randomizeStepsOrder(int array[], int arraySize) {
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	int randomIndex; 

	for (int i = 0; i < arraySize; i++) {
		randomIndex = static_cast<int>(rand() * fraction * (arraySize));
		swap(array[i], array[randomIndex]);
	}	
}

double randomBetween(double min, double max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

double integral(double lastTrendValue, double derivative) {
	return lastTrendValue + derivative;
}

int main(int argc, char* argv[]) {
	
	/* Cчитывание файла */
	ifstream file("C:\\Users\\Evgeniy\\source\\repos\\ft-coder\\entropy_prediction\\entropy_prediction\\data.csv", ios::in);
	vector<double> cursValues; // вектор, в котором хранятся значения тренда
	
	if (!file.is_open()) {
		cerr << "Can not open data.csv file!\n";
		return 1;
	}

	string data, curs;
	while (file >> data >> curs) {
		cursValues.push_back(stod(curs));
	}

	/* Поиск производной */
	vector<double> derivatives; // вектор, в котором хранятся значения производной
	for (int i = 0; i < cursValues.size() - 1; i++) {
		double derivative = cursValues[i + 1] - cursValues[i];
		derivatives.push_back(derivative);
	}


	/* Найти минимальное и максимальное значение производной */
	double minDerivative = derivatives[0];
	double maxDerivative = derivatives[0];
	for (int i = 1; i < derivatives.size() - 1; i++) {
		double temp = derivatives[i];
		if (temp < minDerivative)
			minDerivative = temp;
		if (temp > maxDerivative)
			maxDerivative = temp;
	}

	/* Разделить интервал производной на 999 равных промежутков */
	double* intervals = new double[1000]; //Значения границ промежутков
	double intervalLength = (abs(minDerivative) + abs(maxDerivative)) / 1000.0;
	double temp = minDerivative;
	for (int i = 0; i < 1000; i++) {
		intervals[i] = temp;
		temp += intervalLength;
	}

	/* Подсчитать количество значений производной на каждом промежутке и нормализовать их */
	double* intervalValues = new double[999]; // Количестово значений между промежутками, границы которых лежат в intervals
	double* normalizedIntervalValues = new double[999]; // нормированные на количество точек графика производной значения
	int sumOfIntervalValues = derivatives.size(); // количество точек графика производной
	double intervalBegin, intervalEnd;

	for (int i = 0; i < 999; i++) {
		intervalValues[i] = 0;
		
		intervalBegin = intervals[i];
		intervalEnd = intervals[i + 1];

		for (int j = 0; j < sumOfIntervalValues; j++) {
			double derivative = derivatives[j];
			
			if (derivative >= intervalBegin && derivative < intervalEnd) {
				intervalValues[i]++;
			}
		}

		normalizedIntervalValues[i] = intervalValues[i] / sumOfIntervalValues;
	}

	/* Инициализация поля пробы и поля нормализованных значений пробы */
	double* probe = new double[999];
	double* normalizedProbe = new double[999];

	/* Инициализация порядка проб */
	int* stepOrder = new int[999];
	for (int i = 0; i < 999; i++)
		stepOrder[i] = i;

	// Суммарные недоборы вероятности
	double probeScore, score, probeSum, nextSum;

	/* Инициализация генератора случайных чисел */
	srand(static_cast<unsigned int>(time(0)));

	int steps = 10000; // Количество итераций
	int step, nextStep;

	double lastTrendValue = cursValues.back(); // Нужно для восстановления значения тренда из производной
	double newTrendValue;

	for (int i = 1; i < steps; i++) {
		score = 1e10;

		/* Рандомизация порядка проб */
		randomizeStepsOrder(stepOrder, 999);

		for (int j = 0; j < 999; j++) {
			step = stepOrder[j];

			/* Заполнение пробного поля*/
			probe = intervalValues;
			for (int k = 0; k < 999; k++)
				probe[k] = intervalValues[k];
			
			/* Прибавляем единицу к значению распределения на текущем шаге */
			probe[step]++;
			

			/* Нормализуем пробное поле и подсчитываем суммарный недобор вероятности*/
			probeSum = 0; // сумма точек
			for (int k = 0; k < 999; k++) {
				probeSum += probe[k];
			}

			probeScore = 0.0;
			for (int k = 0; k < 999; k++) {
				normalizedProbe[k] = probe[k] / probeSum;

				if (normalizedProbe[k] < normalizedIntervalValues[k]) {
					probeScore += (normalizedIntervalValues[k] - normalizedProbe[k]);
				}
			}
			
			/* Сравниваем найденный недобор с предыдущим значением и запоминаем его, если он меньше*/
			if (probeScore < score) {
				score = probeScore;
				nextSum = probeSum;
				nextStep = step;
			}
		}

		step = nextStep;
		
		/* 
		   Делаем шаг: определяем границы, в которых лежит прогнозируемое число и генерируем его.
		   Далее берём интеграл на основании предыдущего значения тренда и найденного значения производной
		*/
		newTrendValue = integral(lastTrendValue, randomBetween(intervals[step], intervals[step + 1]));
		lastTrendValue = newTrendValue;

		ofstream outputFile;
		outputFile.open("output.txt", ios::app);
		outputFile << newTrendValue << endl;
		outputFile.close();
	}

	cout << "Gotovo!";

	return 0;
}