#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np

NFFT=131072
CIC_STAGES = 16

F = np.linspace(0, 1, NFFT)

# Retrieve the generated filter and take its FFT
h = np.fromfile('ciccomp.dat', dtype=np.float64)
#h = np.round(h * 131071) / 131071
H = np.abs(np.fft.fft(h, NFFT))

# Calculate the desired amplitude response
Fdes = np.sinc(F)**-CIC_STAGES

# Plot amplitude response vs frequency
plt.plot(F, 20*np.log10(H), 'g', label='Compensation FIR')
plt.plot(np.append(F[F < 0.25], [0.25]),
        np.append(20*np.log10(Fdes[F < 0.25]), [-300]), 'k',
        linewidth=10, alpha=0.2, label='Ideal Response')

plt.plot(F[F < 0.25], 20*np.log10(np.abs(Fdes-H))[F < 0.25],
        'b', label='Passband Error')

plt.xlim([0, 0.5])
plt.ylim([-120, 20])
plt.xticks([0, 0.2, 0.25, 0.5],
    ['0', '$F_\mathrm{Pass}$', '$F_\mathrm{Stop}$', '$F_\mathrm{Nyquist}$'])

plt.title(f'Filter Response, Order {CIC_STAGES} CIC Compensator')
plt.ylabel('Amplitude (dB)')
plt.xlabel('Frequency (normalized)')

plt.grid(True)
plt.legend(borderpad=1, handlelength=0.4, framealpha=1)
plt.savefig('ciccomp_spectrum.png')

# Impulse responses

plt.figure()
plt.stem(np.arange(-len(h)/2, len(h)/2) + 0.5, h,
        markerfmt=',', basefmt='none')
plt.grid()
plt.xlabel('Sample')
plt.ylabel('Amplitude (norm.)')
plt.title(f'Impulse Response, Order {CIC_STAGES} CIC Compensator')
plt.savefig('ciccomp_impulse.png')
