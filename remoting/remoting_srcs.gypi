# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'remoting_base_sources': [
      'base/auth_token_util.cc',
      'base/auth_token_util.h',
      'base/auto_thread.cc',
      'base/auto_thread.h',
      'base/auto_thread_task_runner.cc',
      'base/auto_thread_task_runner.h',
      'base/capabilities.cc',
      'base/capabilities.h',
      'base/compound_buffer.cc',
      'base/compound_buffer.h',
      'base/constants.cc',
      'base/constants.h',
      'base/plugin_thread_task_runner.cc',
      'base/plugin_thread_task_runner.h',
      'base/rate_counter.cc',
      'base/rate_counter.h',
      'base/resources.h',
      'base/resources_linux.cc',
      'base/resources_mac.cc',
      'base/resources_win.cc',
      'base/rsa_key_pair.cc',
      'base/rsa_key_pair.h',
      'base/running_average.cc',
      'base/running_average.h',
      'base/scoped_sc_handle_win.h',
      'base/socket_reader.cc',
      'base/socket_reader.h',
      'base/typed_buffer.h',
      'base/url_request_context.cc',
      'base/url_request_context.h',
      'base/util.cc',
      'base/util.h',
      'base/vlog_net_log.cc',
      'base/vlog_net_log.h',
      'codec/audio_decoder.cc',
      'codec/audio_decoder.h',
      'codec/audio_decoder_opus.cc',
      'codec/audio_decoder_opus.h',
      'codec/audio_decoder_verbatim.cc',
      'codec/audio_decoder_verbatim.h',
      'codec/audio_encoder.h',
      'codec/audio_encoder_opus.cc',
      'codec/audio_encoder_opus.h',
      'codec/audio_encoder_verbatim.cc',
      'codec/audio_encoder_verbatim.h',
      'codec/scoped_vpx_codec.cc',
      'codec/scoped_vpx_codec.h',
      'codec/video_decoder.h',
      'codec/video_decoder_verbatim.cc',
      'codec/video_decoder_verbatim.h',
      'codec/video_decoder_vpx.cc',
      'codec/video_decoder_vpx.h',
      'codec/video_encoder.h',
      'codec/video_encoder_verbatim.cc',
      'codec/video_encoder_verbatim.h',
      'codec/video_encoder_vpx.cc',
      'codec/video_encoder_vpx.h',
    ],

    'remoting_protocol_sources': [
      'jingle_glue/chromium_port_allocator.cc',
      'jingle_glue/chromium_port_allocator.h',
      'jingle_glue/chromium_socket_factory.cc',
      'jingle_glue/chromium_socket_factory.h',
      'jingle_glue/iq_sender.cc',
      'jingle_glue/iq_sender.h',
      'jingle_glue/jingle_info_request.cc',
      'jingle_glue/jingle_info_request.h',
      'jingle_glue/network_settings.cc',
      'jingle_glue/network_settings.h',
      'jingle_glue/signal_strategy.h',
      'jingle_glue/xmpp_signal_strategy.cc',
      'jingle_glue/xmpp_signal_strategy.h',
      'protocol/audio_reader.cc',
      'protocol/audio_reader.h',
      'protocol/audio_stub.h',
      'protocol/audio_writer.cc',
      'protocol/audio_writer.h',
      'protocol/auth_util.cc',
      'protocol/auth_util.h',
      'protocol/authentication_method.cc',
      'protocol/authentication_method.h',
      'protocol/authenticator.cc',
      'protocol/authenticator.h',
      'protocol/buffered_socket_writer.cc',
      'protocol/buffered_socket_writer.h',
      'protocol/channel_authenticator.h',
      'protocol/channel_dispatcher_base.cc',
      'protocol/channel_dispatcher_base.h',
      'protocol/channel_multiplexer.cc',
      'protocol/channel_multiplexer.h',
      'protocol/client_control_dispatcher.cc',
      'protocol/client_control_dispatcher.h',
      'protocol/client_event_dispatcher.cc',
      'protocol/client_event_dispatcher.h',
      'protocol/client_stub.h',
      'protocol/clipboard_echo_filter.cc',
      'protocol/clipboard_echo_filter.h',
      'protocol/clipboard_filter.cc',
      'protocol/clipboard_filter.h',
      'protocol/clipboard_stub.h',
      'protocol/clipboard_thread_proxy.cc',
      'protocol/clipboard_thread_proxy.h',
      'protocol/connection_to_client.cc',
      'protocol/connection_to_client.h',
      'protocol/connection_to_host.cc',
      'protocol/connection_to_host.h',
      'protocol/content_description.cc',
      'protocol/content_description.h',
      'protocol/errors.h',
      'protocol/host_control_dispatcher.cc',
      'protocol/host_control_dispatcher.h',
      'protocol/host_event_dispatcher.cc',
      'protocol/host_event_dispatcher.h',
      'protocol/host_stub.h',
      'protocol/input_event_tracker.cc',
      'protocol/input_event_tracker.h',
      'protocol/input_filter.cc',
      'protocol/input_filter.h',
      'protocol/input_stub.h',
      'protocol/it2me_host_authenticator_factory.cc',
      'protocol/it2me_host_authenticator_factory.h',
      'protocol/jingle_messages.cc',
      'protocol/jingle_messages.h',
      'protocol/jingle_session.cc',
      'protocol/jingle_session.h',
      'protocol/jingle_session_manager.cc',
      'protocol/jingle_session_manager.h',
      'protocol/libjingle_transport_factory.cc',
      'protocol/libjingle_transport_factory.h',
      'protocol/me2me_host_authenticator_factory.cc',
      'protocol/me2me_host_authenticator_factory.h',
      'protocol/message_decoder.cc',
      'protocol/message_decoder.h',
      'protocol/message_reader.cc',
      'protocol/message_reader.h',
      'protocol/message_serialization.cc',
      'protocol/message_serialization.h',
      'protocol/monitored_video_stub.cc',
      'protocol/monitored_video_stub.h',
      'protocol/mouse_input_filter.cc',
      'protocol/mouse_input_filter.h',
      'protocol/name_value_map.h',
      'protocol/negotiating_authenticator_base.cc',
      'protocol/negotiating_authenticator_base.h',
      'protocol/negotiating_client_authenticator.cc',
      'protocol/negotiating_client_authenticator.h',
      'protocol/negotiating_host_authenticator.cc',
      'protocol/negotiating_host_authenticator.h',
      'protocol/pairing_authenticator_base.cc',
      'protocol/pairing_authenticator_base.h',
      'protocol/pairing_client_authenticator.cc',
      'protocol/pairing_client_authenticator.h',
      'protocol/pairing_host_authenticator.cc',
      'protocol/pairing_host_authenticator.h',
      'protocol/pairing_registry.cc',
      'protocol/pairing_registry.h',
      'protocol/protobuf_video_reader.cc',
      'protocol/protobuf_video_reader.h',
      'protocol/protobuf_video_writer.cc',
      'protocol/protobuf_video_writer.h',
      'protocol/session.h',
      'protocol/session_config.cc',
      'protocol/session_config.h',
      'protocol/session_manager.h',
      'protocol/ssl_hmac_channel_authenticator.cc',
      'protocol/ssl_hmac_channel_authenticator.h',
      'protocol/third_party_authenticator_base.cc',
      'protocol/third_party_authenticator_base.h',
      'protocol/third_party_client_authenticator.cc',
      'protocol/third_party_client_authenticator.h',
      'protocol/third_party_host_authenticator.cc',
      'protocol/third_party_host_authenticator.h',
      'protocol/token_validator.h',
      'protocol/transport.cc',
      'protocol/transport.h',
      'protocol/v2_authenticator.cc',
      'protocol/v2_authenticator.h',
      'protocol/video_reader.cc',
      'protocol/video_reader.h',
      'protocol/video_stub.h',
      'protocol/video_writer.cc',
      'protocol/video_writer.h',
    ],

    'remoting_client_sources': [
      'client/audio_decode_scheduler.cc',
      'client/audio_decode_scheduler.h',
      'client/audio_player.cc',
      'client/audio_player.h',
      'client/chromoting_client.cc',
      'client/chromoting_client.h',
      'client/chromoting_stats.cc',
      'client/chromoting_stats.h',
      'client/client_config.cc',
      'client/client_config.h',
      'client/client_context.cc',
      'client/client_context.h',
      'client/client_user_interface.h',
      'client/frame_consumer.h',
      'client/frame_consumer_proxy.cc',
      'client/frame_consumer_proxy.h',
      'client/frame_producer.h',
      'client/key_event_mapper.cc',
      'client/key_event_mapper.h',
      'client/log_to_server.cc',
      'client/log_to_server.h',
      'client/server_log_entry.cc',
      'client/server_log_entry.h',
      'client/software_video_renderer.cc',
      'client/software_video_renderer.h',
      'client/video_renderer.h',
    ],

    'remoting_client_plugin_sources': [
      'client/plugin/chromoting_instance.cc',
      'client/plugin/chromoting_instance.h',
      'client/plugin/delegating_signal_strategy.cc',
      'client/plugin/delegating_signal_strategy.h',
      'client/plugin/media_source_video_renderer.cc',
      'client/plugin/media_source_video_renderer.h',
      'client/plugin/normalizing_input_filter_cros.cc',
      'client/plugin/normalizing_input_filter_cros.h',
      'client/plugin/normalizing_input_filter_mac.cc',
      'client/plugin/normalizing_input_filter_mac.h',
      'client/plugin/pepper_audio_player.cc',
      'client/plugin/pepper_audio_player.h',
      'client/plugin/pepper_input_handler.cc',
      'client/plugin/pepper_input_handler.h',
      'client/plugin/pepper_network_manager.cc',
      'client/plugin/pepper_network_manager.h',
      'client/plugin/pepper_packet_socket_factory.cc',
      'client/plugin/pepper_packet_socket_factory.h',
      'client/plugin/pepper_plugin_thread_delegate.cc',
      'client/plugin/pepper_plugin_thread_delegate.h',
      'client/plugin/pepper_port_allocator.cc',
      'client/plugin/pepper_port_allocator.h',
      'client/plugin/pepper_token_fetcher.cc',
      'client/plugin/pepper_token_fetcher.h',
      'client/plugin/pepper_util.cc',
      'client/plugin/pepper_util.h',
      'client/plugin/pepper_view.cc',
      'client/plugin/pepper_view.h',
    ],
  }
}
