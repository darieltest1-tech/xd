
//Require standard library
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Foundation/Foundation.h>
#include <iostream>
#include <UIKit/UIKit.h>
#include <vector>
#import "pthread.h"
#include <array>
#import <os/log.h>
#include <cmath>
#include <deque>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cerrno>
#include <cctype>
//Imgui library
#import "Esp/CaptainHook.h"
#import "Esp/ImGuiDrawView.h"
#import "IMGUI/imgui.h"
#import "IMGUI/imgui_internal.h"
#import "IMGUI/imgui_impl_metal.h"
#import "IMGUI/zzz.h"
#include "oxorany/oxorany_include.h"
#import "Helper/Mem.h"
#include "font.h"
//import "Hosts/NSObject+URL.h"
#import "Helper/Vector3.h"
#import "Helper/Vector2.h"
#import "Helper/Quaternion.h"
#import "Helper/Monostring.h"
#include "Helper/font.h"
#include "Helper/data.h"
ImFont* verdana_smol;
ImFont* pixel_big = {};
ImFont* pixel_smol = {};
#include "Helper/Obfuscate.h"
#import "Helper/Hooks.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <unistd.h>
#include <string.h>
#include "Other/dobby_defines.h"
#import "Other/H5hook.h"
#include "Other/Paste.h"

// ===== HÀM LẤY TÊN THIẾT BỊ =====
#include <sys/utsname.h>

NSString* getRealDeviceName() {
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceCode = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    NSDictionary *deviceMap = @{
        @"iPhone10,3": @"iPhone X",
        @"iPhone10,6": @"iPhone X",
        @"iPhone11,2": @"iPhone XS",
        @"iPhone11,4": @"iPhone XS Max",
        @"iPhone11,6": @"iPhone XS Max",
        @"iPhone11,8": @"iPhone XR",
        @"iPhone12,1": @"iPhone 11",
        @"iPhone12,3": @"iPhone 11 Pro",
        @"iPhone12,5": @"iPhone 11 Pro Max",
        @"iPhone13,1": @"iPhone 12 mini",
        @"iPhone13,2": @"iPhone 12",
        @"iPhone13,3": @"iPhone 12 Pro",
        @"iPhone13,4": @"iPhone 12 Pro Max",
        @"iPhone14,2": @"iPhone 13 Pro",
        @"iPhone14,3": @"iPhone 13 Pro Max",
        @"iPhone14,4": @"iPhone 13 mini",
        @"iPhone14,5": @"iPhone 13",
        @"iPhone15,2": @"iPhone 14 Pro",
        @"iPhone15,3": @"iPhone 14 Pro Max",
        @"iPhone15,4": @"iPhone 14",
        @"iPhone15,5": @"iPhone 14 Plus",
        @"iPhone16,1": @"iPhone 15 Pro",
        @"iPhone16,2": @"iPhone 15 Pro Max",
        @"iPhone16,3": @"iPhone 15",
        @"iPhone16,4": @"iPhone 15 Plus",
        @"iPhone17,1": @"iPhone 16 Pro",
        @"iPhone17,2": @"iPhone 16 Pro Max",
        @"iPhone17,3": @"iPhone 16",
        @"iPhone17,4": @"iPhone 16 Plus",
        @"iPad13,18": @"iPad 10th Gen",
        @"iPad13,19": @"iPad 10th Gen",
        @"iPad14,1": @"iPad mini 7th Gen",
        @"iPad14,2": @"iPad mini 7th Gen",
        @"iPad14,3": @"iPad Air 11-inch M2",
        @"iPad14,4": @"iPad Air 13-inch M2",
        @"iPad16,1": @"iPad Pro 11-inch M4",
        @"iPad16,2": @"iPad Pro 11-inch M4",
        @"iPad16,3": @"iPad Pro 13-inch M4",
        @"iPad16,4": @"iPad Pro 13-inch M4",
    };
    
    NSString *realName = deviceMap[deviceCode];
    return realName ? realName : deviceCode;
}

#define Hook(x, y, z) \
{ \
    NSString* result_##y = StaticInlineHookPatch(("Frameworks/UnityFramework.framework/UnityFramework"), x, nullptr); \
    if (result_##y) { \
        void* result = StaticInlineHookFunction(("Frameworks/UnityFramework.framework/UnityFramework"), x, (void *) y); \
        *(void **) (&z) = (void*) result; \
    } \
}



#define kWidth  [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height
#define kScale [UIScreen mainScreen].scale

@interface ImGuiDrawView () <MTKViewDelegate>
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@end


@implementation ImGuiDrawView
ImFont *_espFont;
ImFont* verdanab;
ImFont* icons;
ImFont* interb;
ImFont* Urbanist;
static bool MenDeal = true;
static int currentLang = 1; // 0 = English, 1 = Tieng Viet
static float menuAlpha = 0.0f;
static float animProgress = 0.0f;
static float animSpeed = 3.0f;
static int frameCounter = 0;

// ===== HÀM LƯU CÀI ĐẶT (THÊM TRONG CLASS) =====
- (void)saveSettings {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    // Lưu các biến Vars (ESP)
    [defaults setBool:Vars.Enable forKey:@"Vars_Enable"];
    [defaults setBool:Vars.Box forKey:@"Vars_Box"];
    [defaults setBool:Vars.skeleton forKey:@"Vars_Skeleton"];
    [defaults setBool:Vars.lines forKey:@"Vars_Lines"];
    [defaults setBool:Vars.Health forKey:@"Vars_Health"];
    [defaults setBool:Vars.Outline forKey:@"Vars_Outline"];
    [defaults setBool:Vars.OOF forKey:@"Vars_OOF"];
    [defaults setBool:Vars.Name forKey:@"Vars_Name"];
    [defaults setBool:Vars.Distance forKey:@"Vars_Distance"];
    [defaults setBool:Vars.circlepos forKey:@"Vars_CirclePos"];
    [defaults setBool:Vars.enemycount forKey:@"Vars_EnemyCount"];
    
    // Lưu Aimbot
    [defaults setBool:Vars.Aimbot forKey:@"Vars_Aimbot"];
    [defaults setBool:SilentAim forKey:@"SilentAim"];
    [defaults setBool:Vars.VisibleCheck forKey:@"Vars_VisibleCheck"];
    [defaults setBool:Vars.IgnoreKnocked forKey:@"Vars_IgnoreKnocked"];
    
    // Lưu Combo/Slider
    [defaults setInteger:Vars.AimWhen forKey:@"Vars_AimWhen"];
    [defaults setInteger:Vars.AimHitbox forKey:@"Vars_AimHitbox"];
    [defaults setInteger:Vars.AimMode forKey:@"Vars_AimMode"];
    [defaults setFloat:Vars.AimFov forKey:@"Vars_AimFov"];
    
    // Lưu Extra
    [defaults setBool:spin360 forKey:@"spin360"];
    [defaults setFloat:SpinSpeed forKey:@"SpinSpeed"];
    
    // Lưu ngôn ngữ
    [defaults setInteger:currentLang forKey:@"currentLang"];
    
    [defaults synchronize];
}

// Hàm tải cài đặt
- (void)loadSettings {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    // Tải ESP
    Vars.Enable = [defaults boolForKey:@"Vars_Enable"];
    Vars.Box = [defaults boolForKey:@"Vars_Box"];
    Vars.skeleton = [defaults boolForKey:@"Vars_Skeleton"];
    Vars.lines = [defaults boolForKey:@"Vars_Lines"];
    Vars.Health = [defaults boolForKey:@"Vars_Health"];
    Vars.Outline = [defaults boolForKey:@"Vars_Outline"];
    Vars.OOF = [defaults boolForKey:@"Vars_OOF"];
    Vars.Name = [defaults boolForKey:@"Vars_Name"];
    Vars.Distance = [defaults boolForKey:@"Vars_Distance"];
    Vars.circlepos = [defaults boolForKey:@"Vars_CirclePos"];
    Vars.enemycount = [defaults boolForKey:@"Vars_EnemyCount"];
    
    // Tải Aimbot
    Vars.Aimbot = [defaults boolForKey:@"Vars_Aimbot"];
    SilentAim = [defaults boolForKey:@"Aim Silent"];
    Vars.VisibleCheck = [defaults boolForKey:@"Vars_VisibleCheck"];
    Vars.IgnoreKnocked = [defaults boolForKey:@"Vars_IgnoreKnocked"];
    
    // Tải Combo/Slider
    Vars.AimWhen = (int)[defaults integerForKey:@"Vars_AimWhen"];
    Vars.AimHitbox = (int)[defaults integerForKey:@"Vars_AimHitbox"];
    Vars.AimMode = (int)[defaults integerForKey:@"Vars_AimMode"];
    Vars.AimFov = [defaults floatForKey:@"Vars_AimFov"];
    
    // Tải Extra
    spin360 = [defaults boolForKey:@"spin360"];
    SpinSpeed = [defaults floatForKey:@"SpinSpeed"];
    
    // Tải ngôn ngữ
    currentLang = (int)[defaults integerForKey:@"currentLang"];
}
// ===== HẾT PHẦN HÀM =====

- (instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    if (!self.device) abort();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsClassic();
auto& Style = ImGui::GetStyle();

// --- Cấu trúc khung Menu ---
Style.WindowPadding     = ImVec2(12.0f, 12.0f);
Style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
Style.FramePadding      = ImVec2(8.0f, 8.0f);
Style.ScrollbarRounding = 12.0f;
Style.WindowRounding    = 18.0f;
Style.FrameRounding     = 9.0f;
Style.ChildRounding     = 14.0f;
Style.GrabRounding      = 8.0f;
Style.WindowBorderSize  = 1.2f;
Style.ChildBorderSize   = 1.0f;
Style.PopupBorderSize   = 1.0f;
Style.FrameBorderSize   = 0.8f;

// --- Bảng màu TÍM NHẸ + TRONG SUỐT ---
ImVec4* colors = Style.Colors;

// Nền tím nhạt trong suốt
colors[ImGuiCol_WindowBg]             = ImVec4(0.35f, 0.25f, 0.50f, 0.50f); 
colors[ImGuiCol_ChildBg]              = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
colors[ImGuiCol_PopupBg]              = ImVec4(0.15f, 0.10f, 0.20f, 0.90f);

// Viền tím mờ
colors[ImGuiCol_Border]               = ImVec4(0.60f, 0.50f, 0.80f, 0.50f);

// CHỮ TRẮNG (Dễ nhìn hơn trên nền tím)
colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.90f, 1.00f, 1.0f);
colors[ImGuiCol_TextDisabled]         = ImVec4(0.70f, 0.70f, 0.80f, 0.5f);

// Nền tiêu đề tím trong suốt
colors[ImGuiCol_TitleBg]              = ImVec4(0.30f, 0.20f, 0.45f, 0.7f);
colors[ImGuiCol_TitleBgActive]        = ImVec4(0.40f, 0.30f, 0.60f, 0.8f);
colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.20f, 0.15f, 0.30f, 0.5f);

// ===== CÁC THÀNH PHẦN KHÁC TÔNG TÍM =====
colors[ImGuiCol_CheckMark]            = ImVec4(0.80f, 0.70f, 1.00f, 1.0f);   // Checkmark tím sáng

colors[ImGuiCol_SliderGrab]           = ImVec4(0.60f, 0.50f, 0.80f, 0.7f);   // Slider tím mờ
colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.80f, 0.70f, 1.00f, 1.0f);
colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.1f, 0.1f, 0.15f, 0.3f);
colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.4f, 0.3f, 0.6f, 0.4f);
colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.5f, 0.4f, 0.7f, 0.5f);
colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.6f, 0.5f, 0.8f, 0.6f);

colors[ImGuiCol_Button]               = ImVec4(0.40f, 0.30f, 0.60f, 0.4f);
colors[ImGuiCol_ButtonHovered]        = ImVec4(0.50f, 0.40f, 0.75f, 0.6f);
colors[ImGuiCol_ButtonActive]         = ImVec4(0.60f, 0.50f, 0.90f, 0.8f);

colors[ImGuiCol_Header]               = ImVec4(0.40f, 0.30f, 0.60f, 0.4f);
colors[ImGuiCol_HeaderHovered]        = ImVec4(0.50f, 0.40f, 0.75f, 0.6f);
colors[ImGuiCol_HeaderActive]         = ImVec4(0.60f, 0.50f, 0.90f, 0.8f);
colors[ImGuiCol_Tab]                  = ImVec4(0.20f, 0.15f, 0.30f, 0.4f);
colors[ImGuiCol_TabHovered]           = ImVec4(0.50f, 0.40f, 0.75f, 0.6f);
colors[ImGuiCol_TabActive]            = ImVec4(0.40f, 0.30f, 0.60f, 0.8f);

colors[ImGuiCol_FrameBg]              = ImVec4(0.20f, 0.15f, 0.30f, 0.4f);
colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.30f, 0.25f, 0.45f, 0.6f);
colors[ImGuiCol_FrameBgActive]        = ImVec4(0.40f, 0.35f, 0.60f, 0.8f);

colors[ImGuiCol_ResizeGrip]           = ImVec4(0.60f, 0.50f, 0.80f, 0.4f);
colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.70f, 0.60f, 0.90f, 0.6f);
colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.80f, 0.70f, 1.00f, 0.8f);
colors[ImGuiCol_NavHighlight]         = ImVec4(0.80f, 0.70f, 1.00f, 0.8f);

// --- Load Font ---
io.Fonts->AddFontFromMemoryTTF(sansbold, sizeof(sansbold), 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
verdana_smol = io.Fonts->AddFontFromMemoryTTF(verdana, sizeof verdana, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());
pixel_big = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 128, NULL, io.Fonts->GetGlyphRangesCyrillic());
pixel_smol = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 10*2, NULL, io.Fonts->GetGlyphRangesCyrillic());

ImGui_ImplMetal_Init(_device);

    // ===== LOAD CÀI ĐẶT ĐÃ LƯU KHI KHỞI ĐỘNG =====
    [self loadSettings];
    
    return self;
}

+ (void)showChange:(BOOL)open
{
    MenDeal = open;
}

- (MTKView *)mtkView
{
    return (MTKView *)self.view;
}

- (void)loadView
{
    CGFloat w = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width;
    CGFloat h = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height;
    self.view = [[MTKView alloc] initWithFrame:CGRectMake(0, 0, w, h)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mtkView.device = self.device;
    self.mtkView.delegate = self;
    self.mtkView.clearColor = MTLClearColorMake(0, 0, 0, 0);
    self.mtkView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    self.mtkView.clipsToBounds = YES;
    Hook(0x4EB3E88 , BLAGCMCGEJG1, old_BLAGCMCGEJG1);
    Hook(0x56524D4 , old_AutoFire, _AutoFire);
}

#pragma mark - Interaction

- (void)updateIOWithTouchEvent:(UIEvent *)event
{
    UITouch *anyTouch = event.allTouches.anyObject;
    CGPoint touchLocation = [anyTouch locationInView:self.view];
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(touchLocation.x, touchLocation.y);

    BOOL hasActiveTouch = NO;
    for (UITouch *touch in event.allTouches)
    {
        if (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled)
        {
            hasActiveTouch = YES;
            break;
        }
    }
    io.MouseDown[0] = hasActiveTouch;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

#pragma mark - MTKViewDelegate

- (void)drawInMTKView:(MTKView*)view
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;

    CGFloat framebufferScale = view.window.screen.nativeScale ?: UIScreen.mainScreen.nativeScale;
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);
    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ?: 60);
    
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    
    frameCounter++;
    if (frameCounter > 1000) frameCounter = 0;
        
    if (MenDeal == true) {
        [self.view setUserInteractionEnabled:YES];
    } else {
        [self.view setUserInteractionEnabled:NO];
    }

    MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
    if (renderPassDescriptor != nil) {
        id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder pushDebugGroup:@"ImGui Jane"];

        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui::NewFrame();

        CGFloat x = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width) - 400) / 2;
        CGFloat y = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height) - 340) / 2;
        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(360, 290), ImGuiCond_FirstUseEver);

        if (MenDeal) {
            if (animProgress < 1.0f) {
                animProgress += ImGui::GetIO().DeltaTime * animSpeed;
                if (animProgress > 1.0f) animProgress = 1.0f;
            }
        } else {
            if (animProgress > 0.0f) {
                animProgress -= ImGui::GetIO().DeltaTime * animSpeed;
                if (animProgress < 0.0f) animProgress = 0.0f;
            }
        }

        if (animProgress > 0.0f) {
            ImGui::GetStyle().Alpha = animProgress;
            
            float scale = 0.5f + 0.5f * animProgress;
            float baseWidth = 360.0f;
            float baseHeight = 290.0f;
            ImGui::SetNextWindowSize(ImVec2(baseWidth * scale, baseHeight * scale), ImGuiCond_Always);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));                
            ImGui::Begin(oxorany("Telegram: @Gbaovnxios"), &MenDeal, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            
            if (ImGui::BeginTabBar(oxorany("Tab"), ImGuiTabBarFlags_FittingPolicyScroll)) {
                
// --- Tab Main ---
if (ImGui::BeginTabItem("Main")) {
    ImGui::Spacing();
    
    // ===== DÒNG ADMIN MÀU HỒNG ĐẬM =====
    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.6f, 1.0f), oxorany("Project Administrator Creates Telegram @Gbaovnxios ^^"));
    ImGui::Spacing();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7.0f, 7.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);

    // ===== CHIA 2 COT =====
    ImGui::Columns(2, nullptr, false);
    
    // ===== COT TRAI: ESP =====
    if (currentLang == 0) {
        // ===== TIENG ANH =====
        ImGui::Checkbox(oxorany("Enable ESP"), &Vars.Enable);
        ImGui::Checkbox(oxorany("Box 2D"), &Vars.Box);
        ImGui::Checkbox(oxorany("Skeleton"), &Vars.skeleton);
        ImGui::Checkbox(oxorany("Line (Top)"), &Vars.lines);
        ImGui::Checkbox(oxorany("Health Bar"), &Vars.Health);
        ImGui::Checkbox(oxorany("Outline Fix"), &Vars.Outline);
        
        ImGui::Checkbox(oxorany("Show Player"), &Vars.OOF);
        ImGui::Checkbox(oxorany("Name Tag"), &Vars.Name);
        ImGui::Checkbox(oxorany("Show Distance"), &Vars.Distance);
        ImGui::Checkbox(oxorany("3D Field"), &Vars.circlepos);
        ImGui::Checkbox(oxorany("Count Enemies"), &Vars.enemycount);
    } else {
        // ===== TIENG VIET KHONG DAU =====
        ImGui::Checkbox(oxorany("Ve Dinh Vi"), &Vars.Enable);
        ImGui::Checkbox(oxorany("Khung 2D"), &Vars.Box);
        ImGui::Checkbox(oxorany("Bo Xuong"), &Vars.skeleton);
        ImGui::Checkbox(oxorany("Duong Ke"), &Vars.lines);
        ImGui::Checkbox(oxorany("Thanh Mau"), &Vars.Health);
        ImGui::Checkbox(oxorany("Vien Ngoai"), &Vars.Outline);
        
        ImGui::Checkbox(oxorany("Hien Nguoi Choi"), &Vars.OOF);
        ImGui::Checkbox(oxorany("Ten Dich"), &Vars.Name);
        ImGui::Checkbox(oxorany("Khoang Cach"), &Vars.Distance);
        ImGui::Checkbox(oxorany("Vung 3D"), &Vars.circlepos);
        ImGui::Checkbox(oxorany("Dem Ke Dich"), &Vars.enemycount);
    }
    
    ImGui::NextColumn(); // ===== CHUYEN SANG COT PHAI =====
    
    if (currentLang == 0) {

        ImGui::Checkbox("Enable Aimbot", &Vars.Aimbot);
        
        ImGui::Text("Aim When");
        ImGui::Combo("##1", &Vars.AimWhen, Vars.dir, 4);
        
        ImGui::Text("Aim Zone");
        ImGui::Combo("##2", &Vars.AimHitbox, Vars.aimHitboxes, 3);
        
        ImGui::Text("Aim Mode");
        ImGui::Combo("##3", &Vars.AimMode, Vars.aimModes, 3);
        
        if (Vars.AimMode == 2) {
            ImGui::Text("FOV Radius");
            ImGui::SliderFloat("##FOV", &Vars.AimFov, 0.0f, 360.0f, "%.0f");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        
        ImGui::Checkbox("Aim Silent", &SilentAim);
        ImGui::Checkbox("Visible Check", &Vars.VisibleCheck);
        ImGui::Checkbox("Ignore Knocked", &Vars.IgnoreKnocked);
    } else {
        
        ImGui::Checkbox("Bat Aimbot", &Vars.Aimbot);
        
        ImGui::Text("Aim Khi");
        ImGui::Combo("##1", &Vars.AimWhen, Vars.dir, 4);
        
        ImGui::Text("Vung Aim");
        ImGui::Combo("##2", &Vars.AimHitbox, Vars.aimHitboxes, 3);
        
        ImGui::Text("Che Do");
        ImGui::Combo("##3", &Vars.AimMode, Vars.aimModes, 3);
        
        if (Vars.AimMode == 2) {
            ImGui::Text("FOV Radius");
            ImGui::SliderFloat("##FOV", &Vars.AimFov, 0.0f, 360.0f, "%.0f");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        
        ImGui::Checkbox("Im Lang", &SilentAim);
        ImGui::Checkbox("Bo Qua Guc", &Vars.VisibleCheck);
        ImGui::Checkbox("Bo Aim sau Tuong", &Vars.IgnoreKnocked);
    }
    
    ImGui::Columns(1);
    
    ImGui::PopStyleVar(2);
    ImGui::EndTabItem();
}

                // --- Tab EXTRA ---
                if (ImGui::BeginTabItem("Extra")) {
                    ImGui::Spacing();

                    if (currentLang == 0) {
                        ImGui::Checkbox("Speed Spin Bot", &spin360);
                    } else {
                        ImGui::Checkbox("Quay Nhanh", &spin360);
                    }

                    if (spin360) {
                        if (currentLang == 0) {
                            ImGui::SliderFloat("Speed Spin", &SpinSpeed, 0.0f, 30000.0f, "%.0f");
                        } else {
                            ImGui::SliderFloat("Toc Do Quay", &SpinSpeed, 0.0f, 30000.0f, "%.0f");
                        }
                    }

                    ImGui::EndTabItem();
                }

// --- Tab Settings ---
if (ImGui::BeginTabItem("Settings")) {
    ImGui::Spacing();
    
    ImGui::Text("Device: %s", [getRealDeviceName() UTF8String]);
    ImGui::Text("iOS: %s", [[UIDevice currentDevice].systemVersion UTF8String]);
    ImGui::Text("Name: %s", [[UIDevice currentDevice].name UTF8String]);
    ImGui::Text("Version Game: 1.123.1");
    ImGui::Text("Build Menu Version: 1.5.5");
    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), oxorany("Owner: @Gbaovnxios"));;
    
    if (ImGui::Button("Channel")) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://t.me/hachfreefirevnios"]];
    }
    if (ImGui::Button("Admin")) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://t.me/Giabaovnios"]];
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    if (ImGui::Button("Save", ImVec2(80, 35))) {
        [self saveSettings];
    }

    ImGui::Spacing();
    ImGui::Separator();
    
     ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), oxorany("LANGUAGE / NGON NGU"));;
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
    
    if (ImGui::Button("English", ImVec2(80, 35))) {
        currentLang = 0;
        [self saveSettings];
    }
    ImGui::SameLine();
    if (ImGui::Button("Vietnamese", ImVec2(80, 35))) {
        currentLang = 1;
        [self saveSettings];
    }
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    if (currentLang == 0) {
        ImGui::TextDisabled("Selected: English");
    } else {
        ImGui::TextDisabled("Da chon: Tieng Viet");
    }
    
    ImGui::EndTabItem();
}
                ImGui::EndTabBar();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }

        // --- CÁC HÀM LOGIC NGOÀI MENU ---
        if (frameCounter % 2 == 0) {
            get_players();
            draw_watermark();
            aimbot();
            game_sdk->init();
            Vars.isAimFov = (Vars.AimFov > 0);
        }

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
          
        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];
        [commandBuffer presentDrawable:view.currentDrawable];
    }

    [commandBuffer commit];
}

- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size 
{
    // Nội dung để trống
}

@end