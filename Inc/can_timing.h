
#define CAN_SAMPLE_POINT_50
//#define CAN_SAMPLE_POINT_94


#ifdef CAN_SAMPLE_POINT_50
/*
 * Time quanta configuration
 */
#define CAN_SJW     CAN_SJW_1TQ
#define CAN_BS1     CAN_BS1_4TQ
#define CAN_BS2     CAN_BS2_3TQ

/**
 * List of prescalers for the CAN peripheral clock
 * in order to achieve the corresponding bitrates
 * with the above time quanta
 */
enum can_prescaler {
    CAN_PRESCALER_10K = 600,
    CAN_PRESCALER_20K = 300,
    CAN_PRESCALER_50K = 120,
    CAN_PRESCALER_100K = 60,
    CAN_PRESCALER_125K = 48,
    CAN_PRESCALER_250K = 24,
    CAN_PRESCALER_500K = 12,
    CAN_PRESCALER_750K = 8,
    CAN_PRESCALER_1000K = 6,
};
#endif


#ifdef CAN_SAMPLE_POINT_94
#define CAN_SJW     CAN_SJW_1TQ
#define CAN_BS1     CAN_BS1_14TQ
#define CAN_BS2     CAN_BS2_1TQ

enum can_prescaler {
    CAN_PRESCALER_10K = 300,
    CAN_PRESCALER_20K = 150,
    CAN_PRESCALER_50K = 60,
    CAN_PRESCALER_100K = 30,
    CAN_PRESCALER_125K = 24,
    CAN_PRESCALER_250K = 12,
    CAN_PRESCALER_500K = 6,
    CAN_PRESCALER_750K = 4,
    CAN_PRESCALER_1000K = 3,
};
#endif
