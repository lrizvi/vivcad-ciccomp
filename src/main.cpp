#include "firpm/pm.h"
#include <assert.h>

static mpfr::mpreal sxox(const mpfr::mpreal x, double eps=0.001) {
	/* Use approximation when x is small */
	if (mpfr::cmpabs(x, eps) < 0)
		return mpfr::cos(x/2);

	return mpfr::sin(x)/x;
}

PMOutput firpminvsin(std::size_t n, /* filter order */
		double fp, double fs, /* passband, stopband edges; 1 is Fs */
		double wp, double ws, /* passband, stopband weights */
		int order, /* CIC order to compensate */
		mpfr::mpreal eps=0.0001,
		int Nmax=8)
{
	using mpfr::mpreal;

	std::vector<mpfr::mpreal> h;
	std::size_t degree = n / 2u;

	std::vector<mpfr::mpreal> f{0, fp, fs, 0.9999};
	std::vector<mpfr::mpreal> w{wp, ws};

	std::vector<Band> freqBands(w.size());
	std::vector<Band> chebyBands;

	std::vector<mpfr::mpreal> omega(degree + 2u);
	std::vector<mpfr::mpreal> x(degree + 2u);

	if( n % 2 != 0) {
		for(std::size_t i = 0u; i < freqBands.size(); i++) {
			freqBands[i].start = mpfr::const_pi() * f[2u * i];
			freqBands[i].stop  = mpfr::const_pi() * f[2u * i + 1u];
			freqBands[i].space = BandSpace::FREQ;
			freqBands[i].amplitude = [=](BandSpace bSpace, mpfr::mpreal x) -> mpfr::mpreal {
				if(bSpace == BandSpace::CHEBY)
					x = mpfr::acos(x, MPFR_RNDN);
				if(i==0)
					return mpfr::pow(sxox(x/2), -order) / mpfr::cos(x/2);
				else
					return 0;
			};
			freqBands[i].weight = [=](BandSpace bSpace, mpfr::mpreal x) -> mpfr::mpreal {
				if (bSpace == BandSpace::FREQ)
					return mpfr::cos(x / 2) * w[i];
				else
					return mpfr::sqrt((x + 1) / 2) * w[i];
			};
		}
		initUniformExtremas(omega, freqBands, mpfr_get_default_prec());
		applyCos(x, omega);
		bandConversion(chebyBands, freqBands, ConversionDirection::FROMFREQ);
		PMOutput output = exchange(x, chebyBands, eps, Nmax, mpfr_get_default_prec());

		h.resize(n + 1u);
		h[0] = h[n] = output.h[degree] / 4;
		h[degree] = h[degree + 1] = (output.h[0] * 2 + output.h[1]) / 4;
		for(std::size_t i{2u}; i < degree + 1; i++)
			h[degree + 1 - i] = h[degree + i] = (output.h[i - 1] + output.h[i]) / 4u;
		output.h = h;
		return output;

	} else {

		for(std::size_t i = 0u; i < freqBands.size(); i++) {
			freqBands[i].start = mpfr::const_pi() * f[2u * i];
			freqBands[i].stop  = mpfr::const_pi() * f[2u * i + 1u];
			freqBands[i].space = BandSpace::FREQ;
			freqBands[i].amplitude = [=](BandSpace bSpace, mpfr::mpreal x) -> mpfr::mpreal {
				if(bSpace == BandSpace::CHEBY)
					x = mpfr::acos(x, MPFR_RNDN);
				if(i==0)
					return mpfr::pow(sxox(x/2), -order);
				else
					return 0;
			};
			freqBands[i].weight = [=](BandSpace bSpace, mpfr::mpreal x) -> mpfr::mpreal {
				return w[i];
			};
		}

		initUniformExtremas(omega, freqBands, mpfr_get_default_prec());
		applyCos(x, omega);
		bandConversion(chebyBands, freqBands, ConversionDirection::FROMFREQ);
		PMOutput output = exchange(x, chebyBands, eps, Nmax, mpfr_get_default_prec());

		h.resize(n + 1u);
		h[degree] = output.h[0];
		for(std::size_t i{0u}; i < degree; i++)
			h[i] = h[n - i] = output.h[degree - i] / 2u;
		output.h = h;
		return output;
	}
}

int main(int argc, char**argv) {
	auto file = fopen("ciccomp.dat", "w");

	mpfr_set_default_prec(256);

	auto fir = firpminvsin(157, 0.4, 0.5, 1, 1, 16);

	std::cout << "Final Delta	 = " << fir.delta << std::endl;
	std::cout << "Iteration count = " << fir.iter  << std::endl;
	std::cout << "FINISH Parks-McClellan with uniform initialization\n";

	for(auto h: fir.h) {
		auto d = h.toDouble();
		fwrite(&d, sizeof(d), 1, file);
	}
	fclose(file);

	return(0);
}
