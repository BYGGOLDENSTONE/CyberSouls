# ERROR_LOG.md

## Metadata
- **Category**: LOGS
- **Type**: Error Documentation
- **Version**: 1.0
- **Last Updated**: 2025-01-24
- **Dependencies**: Unreal Engine 5.4

## Purpose
Documents common errors, their causes, and solutions encountered during Cybersouls development.

## Common Compilation Errors

### Missing Include Files
**Error**: `fatal error: cannot open source file "cybersouls/Public/[File].h"`

**Cause**: Incorrect include path or missing file

**Solution**:
```cpp
// Use full path from Source folder
#include "cybersouls/Public/Category/FileName.h"
```

### Unresolved External Symbol
**Error**: `error LNK2019: unresolved external symbol`

**Cause**: Missing implementation or module dependency

**Solution**:
1. Ensure .cpp file implements all declared functions
2. Add module to `cybersouls.Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(new string[] { "ModuleName" });
```

### Forward Declaration Issues
**Error**: `incomplete type is not allowed`

**Cause**: Using forward-declared class without full definition

**Solution**:
```cpp
// In .h file
class AMyActor; // Forward declaration

// In .cpp file
#include "MyActor.h" // Full include
```

## Runtime Errors

### Null Pointer Access
**Error**: `Access violation - code c0000005`

**Cause**: Accessing destroyed or uninitialized object

**Solution**:
```cpp
if (IsValid(MyObject))
{
    // Safe to use MyObject
}
```

### Component Not Found
**Error**: `Assertion failed: Component != nullptr`

**Cause**: Component not attached or wrong class

**Solution**:
```cpp
if (UMyComponent* Comp = GetOwner()->FindComponentByClass<UMyComponent>())
{
    // Component found and valid
}
```

### Invalid Cast
**Error**: `Cast of [Object] to [Class] failed`

**Cause**: Object is not of expected type

**Solution**:
```cpp
if (AMyActor* MyActor = Cast<AMyActor>(Actor))
{
    // Cast successful
}
```

## Blueprint Errors

### Missing Blueprint Parent
**Error**: `Blueprint could not be loaded because it derives from an invalid class`

**Cause**: C++ parent class renamed or deleted

**Solution**:
1. Open Blueprint in text editor
2. Update parent class reference
3. Or reparent in Blueprint editor

### Circular Dependency
**Error**: `Circular dependency detected`

**Cause**: Blueprints referencing each other

**Solution**:
1. Use interfaces for communication
2. Use event dispatchers
3. Refactor to remove direct references

### Component Property Reset
**Error**: Component values reset to default after compile

**Cause**: UPROPERTY specifiers incorrect

**Solution**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
float MyValue = 10.0f; // Default value
```

## AI Errors

### Navigation Mesh Issues
**Error**: `AI cannot find path to target`

**Cause**: Missing or invalid NavMesh

**Solution**:
1. Add NavMeshBoundsVolume to level
2. Build navigation (P key)
3. Check RecastNavMesh settings

### Behavior Tree Failures
**Error**: `Behavior tree task failed to execute`

**Cause**: Invalid blackboard key or missing service

**Solution**:
1. Verify blackboard keys match
2. Ensure all tasks return appropriate result
3. Check task preconditions

### Perception Not Working
**Error**: AI doesn't detect player

**Cause**: Perception configuration incorrect

**Solution**:
```cpp
// In AI Controller constructor
PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
PerceptionComponent->ConfigureSense(*SightConfig);
```

## Input System Errors

### Input Not Responding
**Error**: Key presses not triggering actions

**Cause**: Input mapping context not added

**Solution**:
```cpp
if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
{
    Subsystem->AddMappingContext(InputMappingContext, 0);
}
```

### Multiple Input Triggers
**Error**: Action triggers multiple times

**Cause**: Input bound in multiple places

**Solution**:
1. Check for duplicate bindings
2. Use `Started` trigger instead of `Triggered`
3. Clear bindings before rebinding

## Performance Issues

### Frame Rate Drops
**Cause**: Too many line traces or tick functions

**Solution**:
1. Use timers instead of Tick
2. Optimize line trace frequency
3. Profile with Unreal Insights

### Memory Leaks
**Cause**: Objects not garbage collected

**Solution**:
```cpp
// Mark for GC
UPROPERTY()
UObject* MyObject;

// Or manually destroy
MyObject->MarkPendingKill();
```

## Build Errors

### WSL Build Failure
**Error**: Cannot build from WSL

**Solution**: Use Windows Command Prompt:
```bash
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "D:\Unreal Projects\cybersouls\cybersouls.uproject" -Game
```

### Hot Reload Issues
**Error**: Changes not reflected after compile

**Solution**:
1. Close editor
2. Delete Binaries and Intermediate folders
3. Regenerate project files
4. Rebuild

## Common Fixes Checklist

1. **Always Read Before Edit** - Load file contents before modification
2. **Check Death States** - Validate actors/components before use
3. **Verify Pointers** - Use IsValid() for UObjects
4. **Include Paths** - Use full paths from Source folder
5. **Component Access** - Cache and validate component references
6. **Build from Windows** - Not from WSL
7. **Clear Cache** - Delete Intermediate/Binaries when stuck