#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import "RootViewController.h"

@interface AppController : NSObject <UIApplicationDelegate,GKLeaderboardViewControllerDelegate,GKAchievementViewControllerDelegate> {
    UIWindow *window;
}
@property(nonatomic, readonly) RootViewController* viewController;

@end