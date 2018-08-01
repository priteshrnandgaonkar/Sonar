/*
 *  Copyright (c) 2018-present, Facebook, Inc.
 *
 *  This source code is licensed under the MIT license found in the LICENSE
 *  file in the root directory of this source tree.
 *
 */
#if FB_SONARKIT_ENABLED

#import <Foundation/Foundation.h>

#import <SonarKit/SonarPlugin.h>

#import <memory>

@interface SKBufferingPlugin : NSObject<SonarPlugin>

- (instancetype)initWithQueue:(dispatch_queue_t)queue NS_DESIGNATED_INITIALIZER;

- (void)send:(NSString *)method sonarObject:(NSDictionary<NSString *, id> *)sonarObject;

@end

#endif
