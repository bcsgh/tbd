// Simple
large_vel = ((small_mass * small_vel) / (large_mass + small_mass));
MD = (WD + ID);
OD = (MD + WD);
L_max = (L_min + stroke);
large_energy = (((large_mass + small_mass) * std::pow(large_vel, 2)) / 2);
spring_index = (MD / WD);
L_solid = (WD * (N + 1));
K = ((std::pow(WD, 4) * Gs) / ((8 * std::pow(MD, 3)) * N));
// system
F_max = @src[0];
F_min = (((large_energy / stroke) * 2) - F_max);
L_free = ((F_max / K) - (-L_min));
@des[0] = (F_min - ((L_free + (-L_max)) * K));
