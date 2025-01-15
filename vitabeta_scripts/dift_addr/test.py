import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
# import scienceplots  # Assuming you're using the scienceplot style

# Use Seaborn's colorblind-friendly palette
# plt.style.use('science')
# Set random seed for reproducibility
np.random.seed(42)

# Total time duration
total_time = 100
dt = 0.1  # Time step
t = np.arange(0, total_time, dt)

# Define change points (in time units)
change_points = [20, 40, 60, 80]

# Initialize signal array
signal = np.zeros_like(t)

# Phase definitions
# Phase 1: 0 - 20 seconds, Low-frequency sine wave
idx_phase1 = t < change_points[0]
signal[idx_phase1] = np.sin(0.2 * np.pi * t[idx_phase1]) + 0.1 * np.random.randn(np.sum(idx_phase1))

# Phase 2: 20 - 40 seconds, High-frequency sine wave
idx_phase2 = (t >= change_points[0]) & (t < change_points[1])
signal[idx_phase2] = 0.5 * np.sin(2 * np.pi * t[idx_phase2]) + 0.1 * np.random.randn(np.sum(idx_phase2))

# Phase 3: 40 - 60 seconds, Linear increase
idx_phase3 = (t >= change_points[1]) & (t < change_points[2])
signal[idx_phase3] = 0.05 * t[idx_phase3] + 0.5 * np.random.randn(np.sum(idx_phase3))

# Phase 4: 60 - 80 seconds, Exponential decay
idx_phase4 = (t >= change_points[2]) & (t < change_points[3])
signal[idx_phase4] = np.exp(-0.1 * (t[idx_phase4] - change_points[2])) + 0.1 * np.random.randn(np.sum(idx_phase4))

# Phase 5: 80 - 100 seconds, Random noise
idx_phase5 = t >= change_points[3]
signal[idx_phase5] = 0.3 * np.random.randn(np.sum(idx_phase5))

# Plotting
fig, ax = plt.subplots(figsize=(12, 6))

# Plot each phase with different colors
ax.plot(t[idx_phase1], signal[idx_phase1], color='C0', label='Phase 1: Low-frequency Sine Wave')
ax.plot(t[idx_phase2], signal[idx_phase2], color='C1', label='Phase 2: High-frequency Sine Wave')
ax.plot(t[idx_phase3], signal[idx_phase3], color='C2', label='Phase 3: Linear Increase')
ax.plot(t[idx_phase4], signal[idx_phase4], color='C3', label='Phase 4: Exponential Decay')
ax.plot(t[idx_phase5], signal[idx_phase5], color='C4', label='Phase 5: Random Noise')

# Mark change points
for cp in change_points:
    ax.axvline(x=cp, color='k', linestyle='--', linewidth=1)
    ax.text(cp+0.5, ax.get_ylim()[1]*0.9, f'Change Point {cp}s', rotation=90, verticalalignment='top')

# Labels and Title
ax.set_title('Program Memory Behavior with Phase Transitions', fontsize=16)
ax.set_xlabel('Time (s)', fontsize=14)
ax.set_ylabel('Memory Behavior Metric', fontsize=14)

# Legend
handles = [
    mpatches.Patch(color='C0', label='Phase 1: Initialization'),
    mpatches.Patch(color='C1', label='Phase 2: Computation-Intensive'),
    mpatches.Patch(color='C2', label='Phase 3: Memory Allocation'),
    mpatches.Patch(color='C3', label='Phase 4: Cleanup Phase'),
    mpatches.Patch(color='C4', label='Phase 5: Irregular Behavior'),
]
ax.legend(handles=handles, loc='upper right', fontsize=12)

# Grid and Layout
ax.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()

# Save the figure
plt.savefig('program_phase_transitions.pdf', format='pdf', dpi=300)
plt.show()
