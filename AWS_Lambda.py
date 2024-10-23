import json
import boto3

def lambda_handler(event, context):
    # Initialize SNS client
    sns = boto3.client('sns')
    
    # Extract water level from event
    distance = event.get("distance", 0)
    
    # Only send notification if water level exceeds safe threshold
    if distance <= 30:  # Adjust based on your threshold
        phone_number = "+91       "  # Replace with your phone number
        
        # Send SMS via SNS
        response = sns.publish(
            PhoneNumber=phone_number,
            Message=f"Flood Alert! Water level has risen to {distance} cm from the sensor."
        )
    
    return {
        'statusCode': 200,
        'body': json.dumps('Flood alert SMS sent successfully!')
    }
