#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Maze.h"
#include "Algorithms/Backtracker.h"

static int32 CountDeadEnds(const TArray<TArray<uint8>>& Grid)
{
    if (Grid.Num() == 0 || Grid[0].Num() == 0)
    {
        return 0;
    }

    const int32 Height = Grid.Num();
    const int32 Width = Grid[0].Num();
    int32 Count = 0;

    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            if (!Grid[Y][X])
            {
                continue;
            }

            int32 Open = 0;
            if (X > 0 && Grid[Y][X - 1]) ++Open;
            if (X + 1 < Width && Grid[Y][X + 1]) ++Open;
            if (Y > 0 && Grid[Y - 1][X]) ++Open;
            if (Y + 1 < Height && Grid[Y + 1][X]) ++Open;

            if (Open == 1)
            {
                ++Count;
            }
        }
    }
    return Count;
}

// Use fully-qualified flag names so the compiler can resolve them correctly
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FMazeLoopFactorTest,
    "MazeGenerator.LoopFactor.ReducesDeadEnds",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMazeLoopFactorTest::RunTest(const FString& Parameters)
{
    const FIntVector2 Size(25,25);
    const int32 Seed = 12345;
    Backtracker Algorithm;

    TArray<TArray<uint8>> BaseGrid = Algorithm.GetGrid(Size, Seed);

    AMaze* Maze = NewObject<AMaze>();
    Maze->Seed = Seed;

    Maze->MazeGrid = BaseGrid;
    Maze->LoopFactor = 0.f;
    FMath::RandInit(Seed);
    Maze->PostProcessLoopsAndRooms();
    const int32 DeadEndsNoLoops = CountDeadEnds(Maze->MazeGrid);

    Maze->MazeGrid = BaseGrid;
    Maze->LoopFactor = 0.5f;
    FMath::RandInit(Seed);
    Maze->PostProcessLoopsAndRooms();
    const int32 DeadEndsWithLoops = CountDeadEnds(Maze->MazeGrid);

    TestTrue(TEXT("Dead ends decrease when LoopFactor > 0"), DeadEndsWithLoops < DeadEndsNoLoops);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
