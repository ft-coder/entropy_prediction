#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

float heaviside(float x) {
	if (x >= 0)
		return 1.0;
	else
		return 0.0;
}

int main(int argc, char* argv[]) {
	
	/* Cчитывание файла */
	ifstream file("data.csv");
	vector<double> cursValues; // вектор, в котором хранятся значения тренда

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
	
	

	for (int i = 0; i < 999; i++) {
		intervalValues[i] = 0;
		
		double intervalBegin = intervals[i];
		double intervalEnd = intervals[i + 1];

		for (int j = 0; j < sumOfIntervalValues; j++) {
			double derivative = derivatives[j];
			
			if (derivative >= intervalBegin && derivative < intervalEnd) {
				intervalValues[i]++;
			}
		}

		normalizedIntervalValues[i] = intervalValues[i] / sumOfIntervalValues;
	}
	

	int steps = 10000; // Количество итераций
	int current_position = 0; // стартовая позиция, из которой начинаем делать пробы и анализировать их
	int next_position; // следующая позиция

	double* probeField = new double[999];

	
	for (int i = 0; i < steps; i++) {
		double score = 1.0;

		/* Делаем пробы для каждого current_position и считаем суммарный недобор вероятности*/
		for (int j = 0; j < 999; j++) {
			//int nextStep = intervalValues[j];
	
			// Создаём копию распределения и прибавляем +1 к значению распределения на текущем шаге
			for (int k = 0; k < 999; k++) {
				probeField[k] = intervalValues[k];
			}
			probeField[j] += 1;


			int probeSum = sumOfIntervalValues + 1;


			// Нормализируем пробное поле
			double probeScore = 0.0;
			for (int k = 0; k < 999; k++) {
				cout << intervalValues[k] << " " << probeField[k] << endl;
				probeScore += intervalValues[k] - probeField[k];
			}

			

			// Находим минимум
			if (probeScore < score) {
				score = probeScore;
				current_position = j;
				sumOfIntervalValues = probeSum;
			}
		}
	}
	
	

	return 0;
}