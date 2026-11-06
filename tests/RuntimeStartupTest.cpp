#include <gtest/gtest.h>
#include "RuntimeStartup.h"
#include "ivp_physics.hxx"
#include "ivp_environment.hxx"

class IVP_Environment_External_Clock
{
public:
    static double EnergyDamping(IVP_Environment *environment)
    {
        return environment->get_integrated_energy_damp();
    }
};

TEST(RuntimeStartupTest, ColdPhysicsCachesRetailDampingForEitherCrtPrecision)
{
#if defined(_MSC_VER) && defined(_M_IX86)
    struct RestoreWord
    {
        XWORD value = VxGetFPUControlWord();
        ~RestoreWord() { VxSetFPUControlWord(value); }
    } restore;
    const XWORD precisions[] = {0x0200, 0x0300};
    for (XWORD precision : precisions)
    {
        VxSetFPUControlWord(static_cast<XWORD>((restore.value & ~0x0300) | precision));
        InitializePlayerArithmetic();
        IVP_Application_Environment app;
        auto *environment = IVP_Environment_Manager::get_environment_manager()->create_environment(&app, "startup", 0);
        // Captured from the original installation's PC24 constructor. Once
        // cached, changing the precision word on later frames cannot fix it.
        EXPECT_EQ(0.99840492010116577, IVP_Environment_External_Clock::EnergyDamping(environment));
        delete environment;
    }
#else
    GTEST_SKIP() << "Original Win32 startup precision contract";
#endif
}
